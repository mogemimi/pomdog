// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/radian.h"
#include "pomdog/math/degree.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/math/math_functions.h"

namespace pomdog::math {

[[nodiscard]] Radian<float>
toRadians(Degree<float> degrees) noexcept
{
    constexpr auto factor = math::Pi<float> * (1.0f / 180.0f);
    return Radian<float>(degrees.value * factor);
}

[[nodiscard]] Radian<float>
toRadians(float degrees) noexcept
{
    constexpr auto factor = math::Pi<float> * (1.0f / 180.0f);
    return Radian<float>(degrees * factor);
}

[[nodiscard]] Radian<float>
clamp(Radian<float> x, Radian<float> min, Radian<float> max) noexcept
{
    return Radian<float>{clamp(x.value, min.value, max.value)};
}

[[nodiscard]] Radian<float>
saturate(Radian<float> x) noexcept
{
    return Radian<float>{saturate(x.value)};
}

[[nodiscard]] Radian<float>
lerp(Radian<float> source1, Radian<float> source2, float amount) noexcept
{
    return Radian<float>{lerp(source1.value, source2.value, amount)};
}

[[nodiscard]] Radian<float>
smoothstep(Radian<float> min, Radian<float> max, float amount) noexcept
{
    return Radian<float>{smoothstep(min.value, max.value, amount)};
}

} // namespace pomdog::math
