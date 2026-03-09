#include "Atometa/Network/NetworkLayer.h"
#include "Atometa/Core/Logger.h"

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <sstream>

namespace beast = boost::beast;
namespace http  = beast::http;
namespace ws    = beast::websocket;
namespace asio  = boost::asio;
using     tcp   = asio::ip::tcp;

namespace Atometa {

    // =========================================================================
    // NetMessage serialization
    // =========================================================================

    std::string NetMessage::Serialize() const
    {
        json root;
        root["type"] = static_cast<int>(Type);
        root["data"] = Data;
        return root.dump();
    }

    bool NetMessage::Deserialize(const std::string& raw, NetMessage& out)
    {
        try {
            json root = json::parse(raw);
            out.Type  = static_cast<NetMsgType>(root.at("type").get<int>());
            out.Data  = root.at("data");
            return true;
        } catch (...) {
            return false;
        }
    }

    // =========================================================================
    // WsSession — one connected student (server-side)
    // =========================================================================

    struct NetworkLayer::WsSession
        : public std::enable_shared_from_this<WsSession>
    {
        ws::stream<tcp::socket> Socket;
        beast::flat_buffer      Buffer;
        NetworkLayer*           Owner = nullptr;
        std::string             PeerIp;

        explicit WsSession(tcp::socket socket, NetworkLayer* owner)
            : Socket(std::move(socket)), Owner(owner)
        {
            PeerIp = Socket.next_layer()
                           .remote_endpoint()
                           .address()
                           .to_string();
        }

        void Start()
        {
            // WebSocket handshake
            Socket.async_accept([self = shared_from_this()](beast::error_code ec) {
                if (ec) {
                    ATOMETA_WARN("WS accept error: ", ec.message());
                    return;
                }
                if (self->Owner->m_OnConnect)
                    self->Owner->m_OnConnect(self->PeerIp);

                ATOMETA_INFO("Student connected: ", self->PeerIp);
                self->DoRead();
            });
        }

        void DoRead()
        {
            Socket.async_read(Buffer,
                [self = shared_from_this()](beast::error_code ec, std::size_t /*bytes*/)
                {
                    if (ec) {
                        ATOMETA_INFO("Student disconnected: ", self->PeerIp);
                        if (self->Owner->m_OnDisconnect)
                            self->Owner->m_OnDisconnect(self->PeerIp);
                        self->Owner->RemoveSession(self.get());
                        return;
                    }

                    std::string text = beast::buffers_to_string(self->Buffer.data());
                    self->Buffer.consume(self->Buffer.size());

                    NetMessage msg;
                    if (NetMessage::Deserialize(text, msg)) {
                        if (self->Owner->m_OnMessage)
                            self->Owner->m_OnMessage(msg);
                    }

                    self->DoRead(); // keep reading
                });
        }

        void Send(const std::string& text)
        {
            // Post to strand to avoid data races
            asio::post(Socket.get_executor(),
                [self = shared_from_this(), text]() {
                    beast::error_code ec;
                    self->Socket.write(asio::buffer(text), ec);
                    if (ec)
                        ATOMETA_WARN("WS send error: ", ec.message());
                });
        }
    };

    // =========================================================================
    // NetworkLayer
    // =========================================================================

    NetworkLayer::NetworkLayer() = default;

    NetworkLayer::~NetworkLayer()
    {
        StopHost();
        Disconnect();
    }

    // ── Host ─────────────────────────────────────────────────────────────────

    bool NetworkLayer::StartHost(uint16_t port)
    {
        if (m_Running.load()) {
            ATOMETA_WARN("NetworkLayer: already running");
            return false;
        }

        try {
            tcp::endpoint endpoint(tcp::v4(), port);
            m_Acceptor = std::make_unique<tcp::acceptor>(m_IOContext, endpoint);
            m_Role     = NetworkRole::Host;
            m_Running  = true;
            m_Connected = true;

            DoAccept();

            m_IOThread = std::thread([this]() { RunIOContext(); });

            ATOMETA_INFO("Session hosted on port ", port);
            return true;

        } catch (const std::exception& e) {
            ATOMETA_ERROR("StartHost failed: ", e.what());
            return false;
        }
    }

    void NetworkLayer::StopHost()
    {
        if (m_Role != NetworkRole::Host) return;

        m_Running   = false;
        m_Connected = false;
        m_IOContext.stop();

        if (m_IOThread.joinable())
            m_IOThread.join();

        m_Role = NetworkRole::None;
        ATOMETA_INFO("Host session stopped");
    }

    void NetworkLayer::DoAccept()
    {
        m_Acceptor->async_accept(
            [this](beast::error_code ec, tcp::socket socket)
            {
                if (!m_Running.load()) return;

                if (!ec) {
                    auto session = std::make_shared<WsSession>(
                        std::move(socket), this);

                    {
                        std::lock_guard<std::mutex> lock(m_SessionMutex);
                        m_Sessions.push_back(session);
                    }

                    session->Start();
                } else {
                    ATOMETA_WARN("Accept error: ", ec.message());
                }

                DoAccept(); // accept next connection
            });
    }

    void NetworkLayer::BroadcastRaw(const std::string& text)
    {
        std::lock_guard<std::mutex> lock(m_SessionMutex);
        for (auto& session : m_Sessions)
            session->Send(text);
    }

    void NetworkLayer::RemoveSession(WsSession* session)
    {
        std::lock_guard<std::mutex> lock(m_SessionMutex);
        m_Sessions.erase(
            std::remove_if(m_Sessions.begin(), m_Sessions.end(),
                [session](const std::shared_ptr<WsSession>& s) {
                    return s.get() == session;
                }),
            m_Sessions.end());
    }

    // ── Client ────────────────────────────────────────────────────────────────

    bool NetworkLayer::Connect(const std::string& host, uint16_t port)
    {
        if (m_Running.load()) {
            ATOMETA_WARN("NetworkLayer: already running");
            return false;
        }

        m_Role    = NetworkRole::Client;
        m_Running = true;

        m_IOThread = std::thread([this, host, port]() {
            RunClient(host, port);
        });

        return true;
    }

    void NetworkLayer::Disconnect()
    {
        if (m_Role != NetworkRole::Client) return;

        m_Running   = false;
        m_Connected = false;
        m_IOContext.stop();

        if (m_IOThread.joinable())
            m_IOThread.join();

        m_Role = NetworkRole::None;
        ATOMETA_INFO("Disconnected from session");
    }

    void NetworkLayer::RunClient(const std::string& host, uint16_t port)
    {
        try {
            tcp::resolver resolver(m_IOContext);
            auto results = resolver.resolve(host, std::to_string(port));

            tcp::socket socket(m_IOContext);
            asio::connect(socket, results.begin(), results.end());

            ws::stream<tcp::socket> wsStream(std::move(socket));
            wsStream.handshake(host, "/");

            m_Connected = true;
            ATOMETA_INFO("Connected to session at ", host, ":", port);

            if (m_OnConnect)
                m_OnConnect(host);

            ClientReadLoop(wsStream);

        } catch (const std::exception& e) {
            ATOMETA_ERROR("Connect failed: ", e.what());
            m_Connected = false;
            m_Running   = false;
        }
    }

    void NetworkLayer::ClientReadLoop(ws::stream<tcp::socket>& wsStream)
    {
        beast::flat_buffer buffer;

        while (m_Running.load()) {
            beast::error_code ec;
            wsStream.read(buffer, ec);

            if (ec) {
                ATOMETA_INFO("Session closed: ", ec.message());
                break;
            }

            std::string text = beast::buffers_to_string(buffer.data());
            buffer.consume(buffer.size());

            NetMessage msg;
            if (NetMessage::Deserialize(text, msg) && m_OnMessage)
                m_OnMessage(msg);
        }

        m_Connected = false;
        if (m_OnDisconnect)
            m_OnDisconnect("host");
    }

    // ── Shared ────────────────────────────────────────────────────────────────

    void NetworkLayer::Send(const NetMessage& msg)
    {
        std::string text = msg.Serialize();

        if (m_Role == NetworkRole::Host) {
            BroadcastRaw(text);
        }
        // Client sending not needed for MVP (professor only broadcasts)
    }

    uint32_t NetworkLayer::GetClientCount() const
    {
        std::lock_guard<std::mutex> lock(
            const_cast<std::mutex&>(m_SessionMutex));
        return static_cast<uint32_t>(m_Sessions.size());
    }

    void NetworkLayer::RunIOContext()
    {
        while (m_Running.load()) {
            try {
                m_IOContext.run();
                break; // run() returns when no more work
            } catch (const std::exception& e) {
                ATOMETA_ERROR("IO context error: ", e.what());
            }
        }
    }

} // namespace Atometa