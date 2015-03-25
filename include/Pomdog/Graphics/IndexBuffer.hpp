// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_INDEXBUFFER_4119FC34_308A_4EF7_BC78_92FD2D6DBC43_HPP
#define POMDOG_INDEXBUFFER_4119FC34_308A_4EF7_BC78_92FD2D6DBC43_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "BufferUsage.hpp"
#include "IndexElementSize.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeIndexBuffer;

}// namespace RenderSystem
}// namespace Detail

class POMDOG_EXPORT IndexBuffer {
public:
    IndexBuffer() = delete;
    IndexBuffer(IndexBuffer const&) = delete;
    IndexBuffer(IndexBuffer &&) = default;

    IndexBuffer(GraphicsDevice & graphicsDevice,
        IndexElementSize elementSize, void const* indices, std::uint32_t indexCount,
        Pomdog::BufferUsage bufferUsage);

    IndexBuffer(GraphicsDevice & graphicsDevice,
        IndexElementSize elementSize, std::uint32_t indexCount,
        Pomdog::BufferUsage bufferUsage);

    IndexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        IndexElementSize elementSize, void const* indices, std::uint32_t indexCount,
        Pomdog::BufferUsage bufferUsage);

    IndexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        IndexElementSize elementSize, std::uint32_t indexCount,
        Pomdog::BufferUsage bufferUsage);

    ~IndexBuffer();

    IndexBuffer & operator=(IndexBuffer const&) = delete;
    IndexBuffer & operator=(IndexBuffer &&) = default;

    std::uint32_t IndexCount() const;

    IndexElementSize ElementSize() const;

    Pomdog::BufferUsage BufferUsage() const;

    void SetData(void const* source, std::uint32_t elementCount);

    void SetData(std::uint32_t offsetInBytes, void const* source, std::uint32_t elementCount);

public:
    Detail::RenderSystem::NativeIndexBuffer* NativeIndexBuffer();

private:
    std::unique_ptr<Detail::RenderSystem::NativeIndexBuffer> nativeIndexBuffer;
    std::uint32_t indexCount;
    IndexElementSize elementSize;
    Pomdog::BufferUsage bufferUsage;
};

}// namespace Pomdog

#endif // !defined(POMDOG_INDEXBUFFER_4119FC34_308A_4EF7_BC78_92FD2D6DBC43_HPP)
