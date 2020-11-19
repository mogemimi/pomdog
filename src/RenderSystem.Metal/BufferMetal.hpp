// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/BufferBindMode.hpp"
#include "../RenderSystem/NativeBuffer.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Utility/Errors.hpp"
#import <Metal/MTLBuffer.h>

namespace Pomdog::Detail::Metal {

class BufferMetal final : public NativeBuffer {
public:
    [[nodiscard]] std::shared_ptr<Error>
    Initialize(
        id<MTLDevice> device,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        BufferBindMode bindMode) noexcept;

    [[nodiscard]] std::shared_ptr<Error>
    Initialize(
        id<MTLDevice> device,
        const void* vertices,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        BufferBindMode bindMode) noexcept;

    void GetData(
        std::size_t offsetInBytes,
        void* destination,
        std::size_t sizeInBytes) const override;

    void SetData(
        std::size_t offsetInBytes,
        const void* source,
        std::size_t sizeInBytes) override;

    id<MTLBuffer> GetBuffer() const noexcept;

private:
    id<MTLBuffer> nativeBuffer = nullptr;
};

} // namespace Pomdog::Detail::Metal
