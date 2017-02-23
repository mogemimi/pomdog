// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/BufferBindMode.hpp"
#include "../RenderSystem/NativeBuffer.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#import <Metal/MTLBuffer.h>

namespace Pomdog {
namespace Detail {
namespace Metal {

class BufferMetal final : public NativeBuffer {
public:
    BufferMetal(
        id<MTLDevice> device,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        BufferBindMode bindMode);

    BufferMetal(
        id<MTLDevice> device,
        const void* vertices,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        BufferBindMode bindMode);

    void GetData(
        std::size_t offsetInBytes,
        void* destination,
        std::size_t sizeInBytes) const override;

    void SetData(
        std::size_t offsetInBytes,
        const void* source,
        std::size_t sizeInBytes) override;

    id<MTLBuffer> GetBuffer() const;

private:
    id<MTLBuffer> nativeBuffer;
};

} // namespace Metal
} // namespace Detail
} // namespace Pomdog
