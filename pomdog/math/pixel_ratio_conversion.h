// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog {
class Point2D;
class Rect2D;
class Vector2;
} // namespace pomdog

namespace pomdog::math {

/// Converts a logical-pixel point to physical pixels.
/// Multiplies each component by `pixelRatio` and rounds to the nearest integer.
[[nodiscard]] POMDOG_EXPORT Point2D
toPhysicalPixels(const Point2D& logical, f32 pixelRatio) noexcept;

/// Converts a physical-pixel point to logical pixels.
/// Divides each component by `pixelRatio` and rounds to the nearest integer.
[[nodiscard]] POMDOG_EXPORT Point2D
toLogicalPixels(const Point2D& physical, f32 pixelRatio) noexcept;

/// Converts a logical-pixel position to physical pixels without rounding.
[[nodiscard]] POMDOG_EXPORT Vector2
toPhysicalPixels(const Vector2& logical, f32 pixelRatio) noexcept;

/// Converts a physical-pixel position to logical pixels without rounding.
[[nodiscard]] POMDOG_EXPORT Vector2
toLogicalPixels(const Vector2& physical, f32 pixelRatio) noexcept;

/// Converts a logical-pixel rectangle to physical pixels.
/// Each component is multiplied by `pixelRatio` and rounded to the nearest
/// integer.
[[nodiscard]] POMDOG_EXPORT Rect2D
toPhysicalPixels(const Rect2D& logical, f32 pixelRatio) noexcept;

/// Converts a physical-pixel rectangle to logical pixels.
/// Each component is divided by `pixelRatio` and rounded to the nearest
/// integer.
[[nodiscard]] POMDOG_EXPORT Rect2D
toLogicalPixels(const Rect2D& physical, f32 pixelRatio) noexcept;

} // namespace pomdog::math
