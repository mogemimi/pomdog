// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/math/forward_declarations.hpp"
#include "pomdog/math/tagged_arithmetic.hpp"

namespace pomdog::detail {
namespace Tags {

struct DegreeTag {
};

} // namespace Tags
} // namespace pomdog::detail

namespace pomdog::math {

[[nodiscard]] Degree<float> POMDOG_EXPORT
ToDegrees(Radian<float> radians) noexcept;

[[nodiscard]] Degree<float> POMDOG_EXPORT
ToDegrees(float radians) noexcept;

[[nodiscard]] Degree<float> POMDOG_EXPORT
Clamp(Degree<float> x, Degree<float> min, Degree<float> max) noexcept;

[[nodiscard]] Degree<float> POMDOG_EXPORT
Saturate(Degree<float> x) noexcept;

[[nodiscard]] Degree<float> POMDOG_EXPORT
Lerp(Degree<float> source1, Degree<float> source2, float amount) noexcept;

[[nodiscard]] Degree<float> POMDOG_EXPORT
SmoothStep(Degree<float> min, Degree<float> max, float amount) noexcept;

} // namespace pomdog::math
