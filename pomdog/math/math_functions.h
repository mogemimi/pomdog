// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog::math {

/// Clamps a value between a minimum and a maximum.
[[nodiscard]] f32 POMDOG_EXPORT
clamp(f32 x, f32 min, f32 max) noexcept;

/// Clamps a value between a minimum and a maximum.
[[nodiscard]] f64 POMDOG_EXPORT
clamp(f64 x, f64 min, f64 max) noexcept;

/// Clamps a value to the [0, 1] range.
[[nodiscard]] f32 POMDOG_EXPORT
saturate(f32 x) noexcept;

/// Clamps a value to the [0, 1] range.
[[nodiscard]] f64 POMDOG_EXPORT
saturate(f64 x) noexcept;

/// Performs a linear interpolation between two values.
[[nodiscard]] f32 POMDOG_EXPORT
lerp(f32 source1, f32 source2, f32 amount) noexcept;

/// Performs a linear interpolation between two values.
[[nodiscard]] f64 POMDOG_EXPORT
lerp(f64 source1, f64 source2, f64 amount) noexcept;

/// Performs a smooth Hermite interpolation between two values.
[[nodiscard]] f32 POMDOG_EXPORT
smoothstep(f32 min, f32 max, f32 amount) noexcept;

/// Performs a smooth Hermite interpolation between two values.
[[nodiscard]] f64 POMDOG_EXPORT
smoothstep(f64 min, f64 max, f64 amount) noexcept;

/// Returns true if two values are within one ULP of each other.
[[nodiscard]] bool POMDOG_EXPORT
maybeEqual(f32 a, f32 b) noexcept;

/// Returns true if two values are within one ULP of each other.
[[nodiscard]] bool POMDOG_EXPORT
maybeEqual(f64 a, f64 b) noexcept;

/// Returns true if two values are approximately equal within the specified tolerance.
[[nodiscard]] bool POMDOG_EXPORT
approxEqual(f32 a, f32 b, f32 tolerance) noexcept;

/// Returns true if two values are approximately equal using a default tolerance.
[[nodiscard]] bool POMDOG_EXPORT
approxEqual(f32 a, f32 b) noexcept;

/// Returns true if two values are approximately equal within the specified tolerance.
[[nodiscard]] bool POMDOG_EXPORT
approxEqual(f64 a, f64 b, f64 tolerance) noexcept;

/// Returns true if two values are approximately equal using a default tolerance.
[[nodiscard]] bool POMDOG_EXPORT
approxEqual(f64 a, f64 b) noexcept;

} // namespace pomdog::math
