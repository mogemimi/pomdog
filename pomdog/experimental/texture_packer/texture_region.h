// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/math/rect2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

// NOTE:
//                      width
//            <----------------------->
//
//        ^   +-----------------------+
//        |   |         ^             |
//        |   |         |             |
//        |   |         | YOffset     |
//        |   |         v             |
// height |   |<------->+-----------+ |
//        |   | XOffset |           | |
//        |   |         |  Subrect  | |
//        |   |         |           | |
//        |   |         +-----------+ |
//        v   +-----------------------+

struct POMDOG_EXPORT TextureRegion final {
    Rect2D subrect;
    std::int16_t xOffset;
    std::int16_t yOffset;
    std::int16_t width;
    std::int16_t height;
    bool rotate;
};

} // namespace pomdog
