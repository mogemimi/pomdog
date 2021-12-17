// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/math.hpp"
#include "pomdog/utility/assert.hpp"

namespace pomdog::math {

[[nodiscard]] float
Clamp(float x, float min, float max) noexcept
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
Clamp(double x, double min, double max) noexcept
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
Saturate(float x) noexcept
{
    return Clamp(x, 0.0f, 1.0f);
}

[[nodiscard]] double
Saturate(double x) noexcept
{
    return Clamp(x, 0.0, 1.0);
}

[[nodiscard]] float
Lerp(float source1, float source2, float amount) noexcept
{
    return source1 + amount * (source2 - source1);
}

[[nodiscard]] double
Lerp(double source1, double source2, double amount) noexcept
{
    return source1 + amount * (source2 - source1);
}

[[nodiscard]] float
SmoothStep(float min, float max, float amount) noexcept
{
    const auto x = Saturate(amount);
    const auto scale = x * x * (3.0f - 2.0f * x);
    return min + scale * (max - min);
}

[[nodiscard]] double
SmoothStep(double min, double max, double amount) noexcept
{
    const auto x = Saturate(amount);
    const auto scale = x * x * (3.0 - 2.0 * x);
    return min + scale * (max - min);
}

} // namespace pomdog::math
