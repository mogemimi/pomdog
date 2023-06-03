// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/math/forward_declarations.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

struct FontGlyph;

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
