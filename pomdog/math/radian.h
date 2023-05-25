// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/math/forward_declarations.h"
#include "pomdog/math/tagged_arithmetic.h"

namespace pomdog::detail {
namespace Tags {

struct RadianTag {
};

} // namespace Tags
} // namespace pomdog::detail

namespace pomdog::math {

[[nodiscard]] Radian<float> POMDOG_EXPORT
toRadians(Degree<float> degrees) noexcept;

[[nodiscard]] Radian<float> POMDOG_EXPORT
toRadians(float degrees) noexcept;

[[nodiscard]] Radian<float> POMDOG_EXPORT
clamp(Radian<float> x, Radian<float> min, Radian<float> max) noexcept;

[[nodiscard]] Radian<float> POMDOG_EXPORT
saturate(Radian<float> x) noexcept;

[[nodiscard]] Radian<float> POMDOG_EXPORT
lerp(Radian<float> source1, Radian<float> source2, float amount) noexcept;

[[nodiscard]] Radian<float> POMDOG_EXPORT
smoothstep(Radian<float> min, Radian<float> max, float amount) noexcept;

} // namespace pomdog::math
