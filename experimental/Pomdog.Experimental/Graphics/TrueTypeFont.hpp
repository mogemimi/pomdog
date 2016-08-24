// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Graphics/SpriteFont.hpp"
#include <Pomdog/Content/AssetManager.hpp>
#include <Pomdog/Utility/Optional.hpp>
#include <Pomdog/Math/detail/ForwardDeclarations.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <functional>

namespace Pomdog {

class TrueTypeFont final {
public:
    TrueTypeFont(const AssetManager& assets, const std::string& fontPath);

    ~TrueTypeFont();

    Optional<Detail::SpriteFonts::Glyph>
    RasterizeGlyph(
        char32_t codePoint,
        float pixelHeight,
        int textureWidth,
        const std::function<void(int width, int height, Point2D & point, std::uint8_t* & output)>& callback);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog
