// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/ForwardDeclarations.hpp"
#include "Pomdog/Utility/Errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

struct FontGlyph;

class POMDOG_EXPORT TrueTypeFont final {
public:
    TrueTypeFont();

    ~TrueTypeFont();

    [[nodiscard]] std::unique_ptr<Error> Load(const std::string& filePath);

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
