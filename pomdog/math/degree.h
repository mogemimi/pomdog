// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/math/forward_declarations.h"
#include "pomdog/math/tagged_arithmetic.h"

namespace pomdog::detail {
namespace Tags {

struct DegreeTag {
};

} // namespace Tags
} // namespace pomdog::detail

namespace pomdog::math {

[[nodiscard]] Degree<float> POMDOG_EXPORT
toDegrees(Radian<float> radians) noexcept;

[[nodiscard]] Degree<float> POMDOG_EXPORT
toDegrees(float radians) noexcept;

[[nodiscard]] Degree<float> POMDOG_EXPORT
clamp(Degree<float> x, Degree<float> min, Degree<float> max) noexcept;

[[nodiscard]] Degree<float> POMDOG_EXPORT
saturate(Degree<float> x) noexcept;

[[nodiscard]] Degree<float> POMDOG_EXPORT
lerp(Degree<float> source1, Degree<float> source2, float amount) noexcept;

[[nodiscard]] Degree<float> POMDOG_EXPORT
smoothstep(Degree<float> min, Degree<float> max, float amount) noexcept;

} // namespace pomdog::math
