// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog::math {

[[nodiscard]] float POMDOG_EXPORT
clamp(float x, float min, float max) noexcept;

[[nodiscard]] double POMDOG_EXPORT
clamp(double x, double min, double max) noexcept;

[[nodiscard]] float POMDOG_EXPORT
saturate(float x) noexcept;

[[nodiscard]] double POMDOG_EXPORT
saturate(double x) noexcept;

[[nodiscard]] float POMDOG_EXPORT
lerp(float source1, float source2, float amount) noexcept;

[[nodiscard]] double POMDOG_EXPORT
lerp(double source1, double source2, double amount) noexcept;

[[nodiscard]] float POMDOG_EXPORT
smoothstep(float min, float max, float amount) noexcept;

[[nodiscard]] double POMDOG_EXPORT
smoothstep(double min, double max, double amount) noexcept;

} // namespace pomdog::math
