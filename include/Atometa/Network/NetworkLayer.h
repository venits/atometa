#pragma once

#include "Atometa/Core/Core.h"

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>

#include <nlohmann/json.hpp>

#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include <vector>
#include <mutex>
#include <memory>

namespace beast = boost::beast;
namespace asio  = boost::asio;
using     tcp   = asio::ip::tcp;
using     json  = nlohmann::json;

namespace Atometa {

    // ── Message types sent over the wire ─────────────────────────────────
    enum class NetMsgType : uint8_t {
        CameraSync   = 0,   // Professor → Students: camera transform
        NodeSelect   = 1,   // Professor → Students: selected node index
        ChatMessage  = 2,   // Any → All: text message
        SessionInfo  = 3,   // Server → new client: session metadata
        Ping         = 4,
        Pong         = 5,
    };

    struct NetMessage {
        NetMsgType  Type;
        json        Data;

        // Serialize to JSON string
        std::string Serialize() const;

        // Deserialize from JSON string — returns false on parse error
        static bool Deserialize(const std::string& raw, NetMessage& out);
    };

    // ── Callbacks the Application registers ──────────────────────────────
    using OnMessageCallback = std::function<void(const NetMessage&)>;
    using OnConnectCallback = std::function<void(const std::string& peerIp)>;
    using OnDisconnectCallback = std::function<void(const std::string& peerIp)>;

    // ── Role ──────────────────────────────────────────────────────────────
    enum class NetworkRole { None, Host, Client };

    // ── NetworkLayer ──────────────────────────────────────────────────────
    // Single class that can act as either an embedded WS server (Host)
    // or a WS client (Student).  Both run on a background thread so the
    // render loop is never blocked.
    class NetworkLayer {
    public:
        NetworkLayer();
        ~NetworkLayer();

        // ── Host (Professor) ──────────────────────────────────────────
        // Starts embedded WebSocket server on 0.0.0.0:port
        bool StartHost(uint16_t port = 8080);
        void StopHost();

        // ── Client (Student) ──────────────────────────────────────────
        // Connects to professor's machine
        bool Connect(const std::string& host, uint16_t port = 8080);
        void Disconnect();

        // ── Shared ────────────────────────────────────────────────────
        // Send to all connected peers (host broadcasts; client sends to host)
        void Send(const NetMessage& msg);

        NetworkRole GetRole()        const { return m_Role; }
        bool        IsConnected()    const { return m_Connected.load(); }
        uint32_t    GetClientCount() const;

        // ── Callbacks (call before Start/Connect) ─────────────────────
        void SetOnMessage(OnMessageCallback cb)    { m_OnMessage    = std::move(cb); }
        void SetOnConnect(OnConnectCallback cb)    { m_OnConnect    = std::move(cb); }
        void SetOnDisconnect(OnDisconnectCallback cb) { m_OnDisconnect = std::move(cb); }

    private:
        // ── Server internals ──────────────────────────────────────────
        void RunIOContext();
        void DoAccept();

        struct WsSession;
        void BroadcastRaw(const std::string& text);
        void RemoveSession(WsSession* session);

        // ── Client internals ──────────────────────────────────────────
        void RunClient(const std::string& host, uint16_t port);
        void ClientReadLoop(beast::websocket::stream<tcp::socket>& ws);

    private:
        NetworkRole          m_Role      = NetworkRole::None;
        std::atomic<bool>    m_Connected = false;
        std::atomic<bool>    m_Running   = false;

        // Boost.Asio
        asio::io_context     m_IOContext;
        std::thread          m_IOThread;

        // Server state
        std::unique_ptr<tcp::acceptor>  m_Acceptor;
        std::vector<std::shared_ptr<WsSession>> m_Sessions;
        std::mutex                      m_SessionMutex;

        // Callbacks
        OnMessageCallback    m_OnMessage;
        OnConnectCallback    m_OnConnect;
        OnDisconnectCallback m_OnDisconnect;
    };

} // namespace Atometa