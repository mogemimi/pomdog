// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog::math {

[[nodiscard]] f32 POMDOG_EXPORT
clamp(f32 x, f32 min, f32 max) noexcept;

[[nodiscard]] f64 POMDOG_EXPORT
clamp(f64 x, f64 min, f64 max) noexcept;

[[nodiscard]] f32 POMDOG_EXPORT
saturate(f32 x) noexcept;

[[nodiscard]] f64 POMDOG_EXPORT
saturate(f64 x) noexcept;

[[nodiscard]] f32 POMDOG_EXPORT
lerp(f32 source1, f32 source2, f32 amount) noexcept;

[[nodiscard]] f64 POMDOG_EXPORT
lerp(f64 source1, f64 source2, f64 amount) noexcept;

[[nodiscard]] f32 POMDOG_EXPORT
smoothstep(f32 min, f32 max, f32 amount) noexcept;

[[nodiscard]] f64 POMDOG_EXPORT
smoothstep(f64 min, f64 max, f64 amount) noexcept;

} // namespace pomdog::math
