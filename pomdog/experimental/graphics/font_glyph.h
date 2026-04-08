// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/math/rect2d.h"

namespace pomdog {

// NOTE:
//          XAdvance
//  ◄───────────────────────►
//
//  ┌───────────────────────┐
//  │         ▲             │
//  │         │             │
//  │         │ YOffset     │
//  │         ▼             │
//  │◄───────►┌───────────┐ │
//  │ XOffset │           │ │
//  │         │  Subrect  │ │
//  │         │           │ │
//  │         └───────────┘ │
//  └───────────────────────┘

struct FontGlyph final {
    Rect2D subrect;
    i16 xOffset;
    i16 yOffset;
    i16 xAdvance;
    i16 texturePage;
};

} // namespace pomdog
