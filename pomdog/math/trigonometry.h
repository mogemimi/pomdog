// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

// These functions take x in half turns, so for an angle t in turns, sinpi(2 * t)
// is the sine of t. They return exact values at multiples of 0.5, such as
// sinpi(1) == 0 and cospi(0.5) == 0, and NaN for an infinite or NaN x. Other
// results are within about 1.13 ulp of the exact values.

namespace pomdog::math {

/// Returns sin(pi * x).
[[nodiscard]] POMDOG_EXPORT f32
sinpi(f32 x) noexcept;

/// Returns sin(pi * x).
[[nodiscard]] POMDOG_EXPORT f64
sinpi(f64 x) noexcept;

/// Returns cos(pi * x).
[[nodiscard]] POMDOG_EXPORT f32
cospi(f32 x) noexcept;

/// Returns cos(pi * x).
[[nodiscard]] POMDOG_EXPORT f64
cospi(f64 x) noexcept;

/// Returns sin(pi * x) and cos(pi * x), in that order.
[[nodiscard]] POMDOG_EXPORT std::tuple<f32, f32>
sincospi(f32 x) noexcept;

/// Returns sin(pi * x) and cos(pi * x), in that order.
[[nodiscard]] POMDOG_EXPORT std::tuple<f64, f64>
sincospi(f64 x) noexcept;

} // namespace pomdog::math
