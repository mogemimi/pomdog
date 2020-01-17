// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include <cstdint>

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
