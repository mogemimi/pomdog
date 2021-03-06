// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Math/Rectangle.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

// NOTE:
//          XAdvance
//  <----------------------->
//
//  +-----------------------+
//  |         ^             |
//  |         |             |
//  |         | YOffset     |
//  |         v             |
//  |<------->+-----------+ |
//  | XOffset |           | |
//  |         |  Subrect  | |
//  |         |           | |
//  |         +-----------+ |
//  +-----------------------+

struct FontGlyph final {
    Rectangle Subrect;

    // NOTE: UCS-4/UTF-32
    char32_t Character;

    std::int16_t XOffset;
    std::int16_t YOffset;
    std::int16_t XAdvance;
    std::int16_t TexturePage;
};

} // namespace Pomdog
