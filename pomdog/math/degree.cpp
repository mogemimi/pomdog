// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/degree.hpp"
#include "pomdog/math/math.hpp"
#include "pomdog/math/radian.hpp"

namespace pomdog::math {

[[nodiscard]] Degree<float>
ToDegrees(Radian<float> radians) noexcept
{
    constexpr auto factor = 180.0f * (1.0f / math::Pi<float>);
    return Degree<float>{radians.value * factor};
}

[[nodiscard]] Degree<float>
ToDegrees(float radians) noexcept
{
    constexpr auto factor = 180.0f * (1.0f / math::Pi<float>);
    return Degree<float>{radians * factor};
}

[[nodiscard]] Degree<float>
Clamp(Degree<float> x, Degree<float> min, Degree<float> max) noexcept
{
    return Degree<float>{Clamp(x.value, min.value, max.value)};
}

[[nodiscard]] Degree<float>
Saturate(Degree<float> x) noexcept
{
    return Degree<float>{Saturate(x.value)};
}

[[nodiscard]] Degree<float>
Lerp(Degree<float> source1, Degree<float> source2, float amount) noexcept
{
    return Degree<float>{Lerp(source1.value, source2.value, amount)};
}

[[nodiscard]] Degree<float>
SmoothStep(Degree<float> min, Degree<float> max, float amount) noexcept
{
    return Degree<float>{SmoothStep(min.value, max.value, amount)};
}

} // namespace pomdog::math
