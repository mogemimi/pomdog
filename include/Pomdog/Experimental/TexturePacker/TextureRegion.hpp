// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/Rectangle.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

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

} // namespace Pomdog
