// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/math_functions.h"
#include "pomdog/utility/assert.h"

namespace pomdog::math {

[[nodiscard]] float
clamp(float x, float min, float max) noexcept
{
    POMDOG_ASSERT(min < max);
    if (x < min) {
        return min;
    }
    if (x > max) {
        return max;
    }
    return x;
}

[[nodiscard]] double
clamp(double x, double min, double max) noexcept
{
    POMDOG_ASSERT(min < max);
    if (x < min) {
        return min;
    }
    if (x > max) {
        return max;
    }
    return x;
}

[[nodiscard]] float
saturate(float x) noexcept
{
    return clamp(x, 0.0f, 1.0f);
}

[[nodiscard]] double
saturate(double x) noexcept
{
    return clamp(x, 0.0, 1.0);
}

[[nodiscard]] float
lerp(float source1, float source2, float amount) noexcept
{
    return source1 + amount * (source2 - source1);
}

[[nodiscard]] double
lerp(double source1, double source2, double amount) noexcept
{
    return source1 + amount * (source2 - source1);
}

[[nodiscard]] float
smoothstep(float min, float max, float amount) noexcept
{
    const auto x = saturate(amount);
    const auto scale = x * x * (3.0f - 2.0f * x);
    return min + scale * (max - min);
}

[[nodiscard]] double
smoothstep(double min, double max, double amount) noexcept
{
    const auto x = saturate(amount);
    const auto scale = x * x * (3.0 - 2.0 * x);
    return min + scale * (max - min);
}

} // namespace pomdog::math
