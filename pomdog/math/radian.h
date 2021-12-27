// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/math/forward_declarations.hpp"
#include "pomdog/math/tagged_arithmetic.hpp"

namespace pomdog::detail {
namespace Tags {

struct RadianTag {
};

} // namespace Tags
} // namespace pomdog::detail

namespace pomdog::math {

[[nodiscard]] Radian<float> POMDOG_EXPORT
ToRadians(Degree<float> degrees) noexcept;

[[nodiscard]] Radian<float> POMDOG_EXPORT
ToRadians(float degrees) noexcept;

[[nodiscard]] Radian<float> POMDOG_EXPORT
Clamp(Radian<float> x, Radian<float> min, Radian<float> max) noexcept;

[[nodiscard]] Radian<float> POMDOG_EXPORT
Saturate(Radian<float> x) noexcept;

[[nodiscard]] Radian<float> POMDOG_EXPORT
Lerp(Radian<float> source1, Radian<float> source2, float amount) noexcept;

[[nodiscard]] Radian<float> POMDOG_EXPORT
SmoothStep(Radian<float> min, Radian<float> max, float amount) noexcept;

} // namespace pomdog::math
