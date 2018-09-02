// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>

namespace Pomdog {

class AssetManager;
struct FontGlyph;

class POMDOG_EXPORT TrueTypeFont final {
public:
    TrueTypeFont(const AssetManager& assets, const std::string& fontPath);

    ~TrueTypeFont();

    std::optional<FontGlyph> RasterizeGlyph(
        char32_t codePoint,
        float pixelHeight,
        int textureWidth,
        const std::function<void(int width, int height, Point2D& point, std::uint8_t*& output)>& callback);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog
