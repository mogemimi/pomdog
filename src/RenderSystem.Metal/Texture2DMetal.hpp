// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativeTexture2D.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#import <Metal/MTLTexture.h>

namespace Pomdog::Detail::Metal {

class Texture2DMetal final : public NativeTexture2D {
public:
    Texture2DMetal(
        id<MTLDevice> device,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format);

    void SetData(
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format,
        const void* pixelData) override;

    id<MTLTexture> GetTexture() const noexcept;

private:
    id<MTLTexture> texture = nullptr;
};

} // namespace Pomdog::Detail::Metal
