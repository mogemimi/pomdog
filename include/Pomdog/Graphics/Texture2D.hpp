// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Graphics/Texture.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT Texture2D final : public Texture {
public:
    Texture2D() = delete;
    Texture2D(const Texture2D&) = delete;
    Texture2D(Texture2D&&) = default;

    Texture2D(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        std::int32_t width,
        std::int32_t height);

    Texture2D(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        std::int32_t width,
        std::int32_t height,
        bool mipMap,
        SurfaceFormat format);

    ~Texture2D();

    Texture2D& operator=(const Texture2D&) = delete;
    Texture2D& operator=(Texture2D&&) = default;

    std::int32_t GetWidth() const;

    std::int32_t GetHeight() const;

    std::int32_t GetLevelCount() const;

    SurfaceFormat GetFormat() const;

    void SetData(const void* pixelData);

    Detail::NativeTexture2D* GetNativeTexture2D();

private:
    std::unique_ptr<Detail::NativeTexture2D> nativeTexture2D;
    std::int32_t pixelWidth;
    std::int32_t pixelHeight;
    std::int32_t levelCount;
    SurfaceFormat format;
};

} // namespace Pomdog
