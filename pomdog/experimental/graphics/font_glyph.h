// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog {

/// Represents a single glyph in a bitmap font atlas.
///
/// The `subrect` field specifies the rectangle in the texture atlas where the glyph's bitmap is located.
/// The `xOffset` and `yOffset` fields specify how much the glyph should be offset from the current drawing position when rendering.
/// The `xAdvance` field specifies how much to advance the drawing position after rendering this glyph, which may be different from the width of the subrect to allow for spacing between characters.
/// The `texturePage` field specifies which texture page the glyph is located on, in case the font atlas spans multiple textures.
///
/// The layout of glyphs in a line of text can be visualized as follows:
/// ```
///              │         XAdvance         │
///              │◄────────────────────────►│
///
/// ┌────────────┬──────────────────────────┬─────────────┐ ────
/// │            │          ▲               │             │  ▲
/// │            │          │               │             │  │
/// │ ┌──────┐   │          │ YOffset       │             │  │
/// │ │      │   │          │               │             │  │
/// │ │      │   │          ▼               │             │  │
/// │ │      │   │◄────────►┌───────────┐   │             │  │
/// │ │      │   │  XOffset │           │   │             │  │ lineSpacing
/// │ └──────┘   │          │           │   │  ┌────────┐ │  │
/// │            │          │  Subrect  │   │  │        │ │  │
/// │            │          │           │   │  │        │ │  │
/// │            │          │           │   │  └────────┘ │  │
/// │            │          └───────────┘   │             │  │
/// │            │                          │             │  │
/// │ glyph[N-1] │        glyphs[N]         │ glyphs[N+1] │  ▼
/// └────────────┴──────────────────────────┴─────────────┘ ────
/// ```
///
/// The layout of a glyph in the texture atlas can be visualized as follows:
/// ```
///            texture.width
///  │◄───────────────────────────►│
///  │                             │
/// (0,0)
///  ┌─────────────────────────────┐ ────
///  │         ▲                   │  ▲
///  │         │                   │  │
///  │         │y                  │  │
///  │         │                   │  │
///  │         ▼                   │  │
///  │◄───────►┌─────────────────┐ │  │
///  │    x    │(x,y)            │ │  │ texture.height
///  │         │                 │ │  │
///  │         │                 │ │  │
///  │         │     Subrect    h│ │  │
///  │         │                 │ │  │
///  │         │                 │ │  │
///  │         │        w        │ │  │
///  │         └─────────────────┘ │  ▼
///  └─────────────────────────────┘ ────
///                (tex.w-1, tex.h-1)
/// ```
struct FontGlyph final {
    i16 subrectX = 0;
    i16 subrectY = 0;
    i16 subrectWidth = 0;
    i16 subrectHeight = 0;
    i16 xOffset = 0;
    i16 yOffset = 0;
    i16 xAdvance = 0;
    i16 texturePage = 0;
};

} // namespace pomdog
