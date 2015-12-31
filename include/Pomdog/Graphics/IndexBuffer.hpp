// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Graphics/IndexElementSize.hpp"
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
        BufferUsage bufferUsage);

    IndexBuffer(GraphicsDevice & graphicsDevice,
        IndexElementSize elementSize, std::size_t indexCount,
        BufferUsage bufferUsage);

    IndexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        IndexElementSize elementSize,
        void const* indices, std::size_t indexCount,
        BufferUsage bufferUsage);

    IndexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        IndexElementSize elementSize, std::size_t indexCount,
        BufferUsage bufferUsage);

    ~IndexBuffer();

    IndexBuffer & operator=(IndexBuffer const&) = delete;
    IndexBuffer & operator=(IndexBuffer &&) = default;

    std::size_t GetIndexCount() const noexcept;

    IndexElementSize GetElementSize() const noexcept;

    std::size_t GetSizeInBytes() const noexcept;

    BufferUsage GetBufferUsage() const noexcept;

    void SetData(void const* source, std::size_t elementCount);

    void SetData(std::size_t offsetInBytes,
        void const* source, std::size_t elementCount);

public:
    Detail::NativeBuffer* NativeIndexBuffer();

private:
    std::unique_ptr<Detail::NativeBuffer> nativeIndexBuffer;
    std::uint32_t indexCount;
    IndexElementSize elementSize;
    BufferUsage bufferUsage;
};

} // namespace Pomdog
