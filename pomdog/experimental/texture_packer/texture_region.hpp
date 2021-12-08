// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/math/rectangle.hpp"

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
    Rectangle Subrect;
    std::int16_t XOffset;
    std::int16_t YOffset;
    std::int16_t Width;
    std::int16_t Height;
    bool Rotate;
};

} // namespace pomdog
