// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Graphics/IndexElementSize.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstddef>
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT IndexBuffer final {
public:
    IndexBuffer() = delete;
    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer(IndexBuffer&&) = default;

    IndexBuffer(
        GraphicsDevice& graphicsDevice,
        IndexElementSize elementSize,
        const void* indices,
        std::size_t indexCount,
        BufferUsage bufferUsage);

    IndexBuffer(
        GraphicsDevice& graphicsDevice,
        IndexElementSize elementSize,
        std::size_t indexCount,
        BufferUsage bufferUsage);

    IndexBuffer(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        IndexElementSize elementSize,
        const void* indices,
        std::size_t indexCount,
        BufferUsage bufferUsage);

    IndexBuffer(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        IndexElementSize elementSize,
        std::size_t indexCount,
        BufferUsage bufferUsage);

    ~IndexBuffer();

    IndexBuffer& operator=(const IndexBuffer&) = delete;
    IndexBuffer& operator=(IndexBuffer&&) = default;

    std::size_t GetIndexCount() const noexcept;

    IndexElementSize GetElementSize() const noexcept;

    std::size_t GetSizeInBytes() const noexcept;

    BufferUsage GetBufferUsage() const noexcept;

    void SetData(const void* source, std::size_t elementCount);

    void SetData(
        std::size_t offsetInBytes,
        const void* source,
        std::size_t elementCount);

public:
    Detail::NativeBuffer* NativeIndexBuffer();

private:
    std::unique_ptr<Detail::NativeBuffer> nativeIndexBuffer;
    std::uint32_t indexCount;
    IndexElementSize elementSize;
    BufferUsage bufferUsage;
};

} // namespace Pomdog
