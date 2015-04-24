// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_INDEXBUFFER_4119FC34_HPP
#define POMDOG_INDEXBUFFER_4119FC34_HPP

#include "detail/ForwardDeclarations.hpp"
#include "BufferUsage.hpp"
#include "IndexElementSize.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstddef>
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT IndexBuffer {
public:
    IndexBuffer() = delete;
    IndexBuffer(IndexBuffer const&) = delete;
    IndexBuffer(IndexBuffer &&) = default;

    IndexBuffer(GraphicsDevice & graphicsDevice,
        IndexElementSize elementSize,
        void const* indices, std::size_t indexCount,
        Pomdog::BufferUsage bufferUsage);

    IndexBuffer(GraphicsDevice & graphicsDevice,
        IndexElementSize elementSize, std::size_t indexCount,
        Pomdog::BufferUsage bufferUsage);

    IndexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        IndexElementSize elementSize,
        void const* indices, std::size_t indexCount,
        Pomdog::BufferUsage bufferUsage);

    IndexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        IndexElementSize elementSize, std::size_t indexCount,
        Pomdog::BufferUsage bufferUsage);

    ~IndexBuffer();

    IndexBuffer & operator=(IndexBuffer const&) = delete;
    IndexBuffer & operator=(IndexBuffer &&) = default;

    std::size_t IndexCount() const noexcept;

    IndexElementSize ElementSize() const noexcept;

    std::size_t SizeInBytes() const noexcept;

    Pomdog::BufferUsage BufferUsage() const noexcept;

    void SetData(void const* source, std::size_t elementCount);

    void SetData(std::size_t offsetInBytes,
        void const* source, std::size_t elementCount);

public:
    Detail::NativeBuffer* NativeIndexBuffer();

private:
    std::unique_ptr<Detail::NativeBuffer> nativeIndexBuffer;
    std::uint32_t indexCount;
    IndexElementSize elementSize;
    Pomdog::BufferUsage bufferUsage;
};

} // namespace Pomdog

#endif // POMDOG_INDEXBUFFER_4119FC34_HPP
