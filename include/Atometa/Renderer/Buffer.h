#pragma once

#include "Atometa/Core/Core.h"
#include <vector>

namespace Atometa {

    enum class BufferType {
        Vertex,
        Index
    };

    enum class BufferUsage {
        Static,   // Data will not change
        Dynamic,  // Data will change occasionally
        Stream    // Data will change every frame
    };

    class VertexBuffer {
    public:
        VertexBuffer(const void* data, uint32_t size, BufferUsage usage = BufferUsage::Static);
        ~VertexBuffer();

        void Bind() const;
        void Unbind() const;

        void SetData(const void* data, uint32_t size);

        uint32_t GetRendererID() const { return m_RendererID; }

    private:
        uint32_t m_RendererID;
    };

    class IndexBuffer {
    public:
        IndexBuffer(const uint32_t* indices, uint32_t count, BufferUsage usage = BufferUsage::Static);
        ~IndexBuffer();

        void Bind() const;
        void Unbind() const;

        void SetData(const uint32_t* indices, uint32_t count);

        uint32_t GetCount() const { return m_Count; }
        uint32_t GetRendererID() const { return m_RendererID; }

    private:
        uint32_t m_RendererID;
        uint32_t m_Count;
    };

}