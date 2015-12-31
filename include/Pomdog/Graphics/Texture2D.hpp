// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/Texture.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT Texture2D : public Texture {
public:
    Texture2D() = delete;
    Texture2D(Texture2D const&) = delete;
    Texture2D(Texture2D &&) = default;

    Texture2D(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        std::int32_t width, std::int32_t height);

    Texture2D(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        std::int32_t width, std::int32_t height, bool mipMap, SurfaceFormat format);

    ~Texture2D();

    Texture2D & operator=(Texture2D const&) = delete;
    Texture2D & operator=(Texture2D &&) = default;

    std::int32_t GetWidth() const;

    std::int32_t GetHeight() const;

    std::int32_t GetLevelCount() const;

    SurfaceFormat GetFormat() const;

    void SetData(void const* pixelData);

    Detail::NativeTexture2D* GetNativeTexture2D();

private:
    std::unique_ptr<Detail::NativeTexture2D> nativeTexture2D;
    std::int32_t pixelWidth;
    std::int32_t pixelHeight;
    std::int32_t levelCount;
    SurfaceFormat format;
};

} // namespace Pomdog
