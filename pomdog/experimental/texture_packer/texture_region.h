// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog {

/// TextureRegion describes a sprite's location in a texture atlas and its
/// relationship to its original framing rectangle.
///
/// When tight packing is enabled at build time, transparent margins are trimmed
/// from each sprite before packing.  The xOffset and yOffset fields record how
/// much was trimmed from the left and top edges respectively, so the engine can
/// reconstruct the original sprite position at draw time.
///
/// The layout of a sprite within its original framing rectangle:
/// ```
///            region.width
///  │◄───────────────────────────►│
///  │                             │
/// (0,0)
///  ┌─────────────────────────────┐ ────
///  │         ▲                   │  ▲
///  │         │                   │  │
///  │         │ yOffset           │  │
///  │         │                   │  │
///  │         ▼                   │  │
///  │◄───────►┌─────────────────┐ │  │
///  │ xOffset │                 │ │  │ region.height
///  │         │                 │ │  │
///  │         │                 │ │  │
///  │         │     subrect    h│ │  │
///  │         │                 │ │  │
///  │         │                 │ │  │
///  │         │        w        │ │  │
///  │         └─────────────────┘ │  ▼
///  └─────────────────────────────┘ ────
///                (region.w-1, region.h-1)
/// ```
///
/// The location of subrect within the texture atlas:
/// ```
///            texture.width
///  │◄───────────────────────────►│
///  │                             │
/// (0,0)
///  ┌─────────────────────────────┐ ────
///  │         ▲                   │  ▲
///  │         │                   │  │
///  │         │ y                 │  │
///  │         │                   │  │
///  │         ▼                   │  │
///  │◄───────►┌─────────────────┐ │  │
///  │    x    │(x,y)            │ │  │ texture.height
///  │         │                 │ │  │
///  │         │                 │ │  │
///  │         │     subrect    h│ │  │
///  │         │                 │ │  │
///  │         │                 │ │  │
///  │         │        w        │ │  │
///  │         └─────────────────┘ │  ▼
///  └─────────────────────────────┘ ────
///                (tex.w-1, tex.h-1)
/// ```
struct POMDOG_EXPORT TextureRegion final {
    i16 subrectX = 0;
    i16 subrectY = 0;
    i16 subrectWidth = 0;
    i16 subrectHeight = 0;
    i16 xOffset = 0;
    i16 yOffset = 0;
    i16 width = 0;
    i16 height = 0;
    bool rotate = false;
};

} // namespace pomdog
