// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/degree.h"
#include "pomdog/math/math.h"
#include "pomdog/math/radian.h"

namespace pomdog::math {

[[nodiscard]] Degree<float>
toDegrees(Radian<float> radians) noexcept
{
    constexpr auto factor = 180.0f * (1.0f / math::Pi<float>);
    return Degree<float>{radians.value * factor};
}

[[nodiscard]] Degree<float>
toDegrees(float radians) noexcept
{
    constexpr auto factor = 180.0f * (1.0f / math::Pi<float>);
    return Degree<float>{radians * factor};
}

[[nodiscard]] Degree<float>
clamp(Degree<float> x, Degree<float> min, Degree<float> max) noexcept
{
    return Degree<float>{clamp(x.value, min.value, max.value)};
}

[[nodiscard]] Degree<float>
saturate(Degree<float> x) noexcept
{
    return Degree<float>{saturate(x.value)};
}

[[nodiscard]] Degree<float>
lerp(Degree<float> source1, Degree<float> source2, float amount) noexcept
{
    return Degree<float>{lerp(source1.value, source2.value, amount)};
}

[[nodiscard]] Degree<float>
smoothstep(Degree<float> min, Degree<float> max, float amount) noexcept
{
    return Degree<float>{smoothstep(min.value, max.value, amount)};
}

} // namespace pomdog::math
