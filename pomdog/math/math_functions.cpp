// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/math_functions.h"
#include "pomdog/utility/assert.h"

namespace pomdog::math {

[[nodiscard]] f32
clamp(f32 x, f32 min, f32 max) noexcept
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

[[nodiscard]] f64
clamp(f64 x, f64 min, f64 max) noexcept
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

[[nodiscard]] f32
saturate(f32 x) noexcept
{
    return clamp(x, 0.0f, 1.0f);
}

[[nodiscard]] f64
saturate(f64 x) noexcept
{
    return clamp(x, 0.0, 1.0);
}

[[nodiscard]] f32
lerp(f32 source1, f32 source2, f32 amount) noexcept
{
    return source1 + amount * (source2 - source1);
}

[[nodiscard]] f64
lerp(f64 source1, f64 source2, f64 amount) noexcept
{
    return source1 + amount * (source2 - source1);
}

[[nodiscard]] f32
smoothstep(f32 min, f32 max, f32 amount) noexcept
{
    const auto x = saturate(amount);
    const auto scale = x * x * (3.0f - 2.0f * x);
    return min + scale * (max - min);
}

[[nodiscard]] f64
smoothstep(f64 min, f64 max, f64 amount) noexcept
{
    const auto x = saturate(amount);
    const auto scale = x * x * (3.0 - 2.0 * x);
    return min + scale * (max - min);
}

} // namespace pomdog::math
