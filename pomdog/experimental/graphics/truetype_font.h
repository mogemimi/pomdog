// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <optional>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Point2D;
class Error;
struct FontGlyph;
} // namespace pomdog

namespace pomdog {

class POMDOG_EXPORT TrueTypeFont final {
public:
    TrueTypeFont();

    ~TrueTypeFont();

    [[nodiscard]] std::unique_ptr<Error>
    load(const std::string& filePath);

    std::optional<FontGlyph>
    rasterizeGlyph(
        char32_t codePoint,
        float pixelHeight,
        int textureWidth,
        const std::function<void(int width, int height, Point2D& point, std::uint8_t*& output)>& callback);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace pomdog
