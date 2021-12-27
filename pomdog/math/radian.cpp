// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/radian.h"
#include "pomdog/math/degree.h"
#include "pomdog/math/math.h"

namespace pomdog::math {

[[nodiscard]] Radian<float>
ToRadians(Degree<float> degrees) noexcept
{
    constexpr auto factor = math::Pi<float> * (1.0f / 180.0f);
    return Radian<float>(degrees.value * factor);
}

[[nodiscard]] Radian<float>
ToRadians(float degrees) noexcept
{
    constexpr auto factor = math::Pi<float> * (1.0f / 180.0f);
    return Radian<float>(degrees * factor);
}

[[nodiscard]] Radian<float>
Clamp(Radian<float> x, Radian<float> min, Radian<float> max) noexcept
{
    return Radian<float>{Clamp(x.value, min.value, max.value)};
}

[[nodiscard]] Radian<float>
Saturate(Radian<float> x) noexcept
{
    return Radian<float>{Saturate(x.value)};
}

[[nodiscard]] Radian<float>
Lerp(Radian<float> source1, Radian<float> source2, float amount) noexcept
{
    return Radian<float>{Lerp(source1.value, source2.value, amount)};
}

[[nodiscard]] Radian<float>
SmoothStep(Radian<float> min, Radian<float> max, float amount) noexcept
{
    return Radian<float>{SmoothStep(min.value, max.value, amount)};
}

} // namespace pomdog::math
