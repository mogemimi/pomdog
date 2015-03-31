// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TEXTURE2D_F041ED3F_HPP
#define POMDOG_TEXTURE2D_F041ED3F_HPP

#include "detail/ForwardDeclarations.hpp"
#include "Texture.hpp"
#include "SurfaceFormat.hpp"
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

    std::int32_t Width() const;

    std::int32_t Height() const;

    std::int32_t LevelCount() const;

    SurfaceFormat Format() const;

    void SetData(void const* pixelData);

public:
    Detail::RenderSystem::NativeTexture2D* NativeTexture2D();

private:
    std::unique_ptr<Detail::RenderSystem::NativeTexture2D> nativeTexture2D;
    std::int32_t pixelWidth;
    std::int32_t pixelHeight;
    std::int32_t levelCount;
    SurfaceFormat format;
};

} // namespace Pomdog

#endif // POMDOG_TEXTURE2D_F041ED3F_HPP
