// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/vector_xz.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/vector2.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
#include <limits>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

static_assert(std::is_trivially_copyable_v<VectorXZ>);
static_assert(std::is_standard_layout_v<VectorXZ>);

VectorXZ::VectorXZ() noexcept = default;

VectorXZ::VectorXZ(f32 xIn, f32 zIn) noexcept
    : x(xIn)
    , z(zIn)
{
}

VectorXZ& VectorXZ::operator+=(const VectorXZ& v) noexcept
{
    x += v.x;
    z += v.z;
    return *this;
}

VectorXZ& VectorXZ::operator-=(const VectorXZ& v) noexcept
{
    x -= v.x;
    z -= v.z;
    return *this;
}

VectorXZ& VectorXZ::operator*=(f32 factor) noexcept
{
    x *= factor;
    z *= factor;
    return *this;
}

VectorXZ& VectorXZ::operator/=(f32 factor) noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    x /= factor;
    z /= factor;
    return *this;
}

VectorXZ VectorXZ::operator+() const noexcept
{
    return *this;
}

VectorXZ VectorXZ::operator-() const noexcept
{
    return VectorXZ{-x, -z};
}

VectorXZ VectorXZ::operator+(const VectorXZ& v) const noexcept
{
    return VectorXZ{x + v.x, z + v.z};
}

VectorXZ VectorXZ::operator-(const VectorXZ& v) const noexcept
{
    return VectorXZ{x - v.x, z - v.z};
}

VectorXZ VectorXZ::operator*(const VectorXZ& v) const noexcept
{
    return VectorXZ{x * v.x, z * v.z};
}

VectorXZ VectorXZ::operator/(const VectorXZ& v) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(v.x) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(v.z) != FP_ZERO);
    return VectorXZ{x / v.x, z / v.z};
}

VectorXZ VectorXZ::operator*(f32 factor) const noexcept
{
    return VectorXZ{x * factor, z * factor};
}

VectorXZ VectorXZ::operator/(f32 factor) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    return VectorXZ{x / factor, z / factor};
}

bool VectorXZ::operator==(const VectorXZ& other) const noexcept
{
    return (x == other.x) && (z == other.z);
}

bool VectorXZ::operator!=(const VectorXZ& other) const noexcept
{
    return (x != other.x) || (z != other.z);
}

const f32* VectorXZ::data() const noexcept
{
    return &x;
}

[[nodiscard]] VectorXZ
operator*(f32 factor, VectorXZ v) noexcept
{
    return VectorXZ{factor * v.x, factor * v.z};
}

} // namespace pomdog

namespace pomdog::math {

[[nodiscard]] VectorXZ
abs(VectorXZ v) noexcept
{
    return VectorXZ{std::abs(v.x), std::abs(v.z)};
}

[[nodiscard]] VectorXZ
floor(VectorXZ v) noexcept
{
    return VectorXZ{std::floor(v.x), std::floor(v.z)};
}

[[nodiscard]] VectorXZ
round(VectorXZ v) noexcept
{
    return VectorXZ{std::round(v.x), std::round(v.z)};
}

[[nodiscard]] VectorXZ
ceil(VectorXZ v) noexcept
{
    return VectorXZ{std::ceil(v.x), std::ceil(v.z)};
}

[[nodiscard]] f32
length(VectorXZ v) noexcept
{
    return std::sqrt(v.x * v.x + v.z * v.z);
}

[[nodiscard]] f32
lengthSquared(VectorXZ v) noexcept
{
    return v.x * v.x + v.z * v.z;
}

[[nodiscard]] f32
distance(VectorXZ a, VectorXZ b) noexcept
{
    return length(a - b);
}

[[nodiscard]] f32
distanceSquared(VectorXZ a, VectorXZ b) noexcept
{
    return lengthSquared(a - b);
}

[[nodiscard]] f32
dot(VectorXZ a, VectorXZ b) noexcept
{
    return a.x * b.x + a.z * b.z;
}

[[nodiscard]] f32
cross(VectorXZ a, VectorXZ b) noexcept
{
    return a.x * b.z - a.z * b.x;
}

[[nodiscard]] VectorXZ
normalize(VectorXZ v) noexcept
{
    const auto len = length(v);

    if (len > std::numeric_limits<decltype(len)>::epsilon()) {
        POMDOG_ASSERT(std::fpclassify(len) != FP_ZERO);
        POMDOG_ASSERT(std::fpclassify(len) != FP_NAN);
        POMDOG_ASSERT(std::fpclassify(len) != FP_INFINITE);
        const auto inverseLength = 1.0f / len;
        return v * inverseLength;
    }
    return v;
}

[[nodiscard]] VectorXZ
min(VectorXZ a, VectorXZ b) noexcept
{
    return VectorXZ{
        std::min(a.x, b.x),
        std::min(a.z, b.z),
    };
}

[[nodiscard]] VectorXZ
max(VectorXZ a, VectorXZ b) noexcept
{
    return VectorXZ{
        std::max(a.x, b.x),
        std::max(a.z, b.z),
    };
}

[[nodiscard]] VectorXZ
saturate(VectorXZ v) noexcept
{
    return clamp(v, VectorXZ{0.0f, 0.0f}, VectorXZ{1.0f, 1.0f});
}

[[nodiscard]] VectorXZ
clamp(VectorXZ source, VectorXZ min, VectorXZ max) noexcept
{
    return VectorXZ{
        math::clamp(source.x, min.x, max.x),
        math::clamp(source.z, min.z, max.z),
    };
}

[[nodiscard]] VectorXZ
lerp(VectorXZ source1, VectorXZ source2, f32 amount) noexcept
{
    return VectorXZ{
        math::lerp(source1.x, source2.x, amount),
        math::lerp(source1.z, source2.z, amount),
    };
}

[[nodiscard]] VectorXZ
smoothstep(VectorXZ source1, VectorXZ source2, f32 amount) noexcept
{
    return VectorXZ{
        math::smoothstep(source1.x, source2.x, amount),
        math::smoothstep(source1.z, source2.z, amount),
    };
}

[[nodiscard]] VectorXZ
rotate(VectorXZ v, Radian<f32> radian) noexcept
{
    const auto sin = std::sin(radian.get());
    const auto cos = std::cos(radian.get());
    return VectorXZ{
        (cos * v.x) - (sin * v.z),
        (sin * v.x) + (cos * v.z),
    };
}

[[nodiscard]] Radian<f32>
toPositiveAngle(VectorXZ v) noexcept
{
    return toSignedAngle(v) + math::Pi<f32>;
}

[[nodiscard]] Radian<f32>
toSignedAngle(VectorXZ v) noexcept
{
    return std::atan2(v.z, v.x);
}

[[nodiscard]] Vector2
toVector2(VectorXZ v) noexcept
{
    return Vector2{v.x, v.z};
}

[[nodiscard]] VectorXZ
toVectorXZ(Vector2 v) noexcept
{
    return VectorXZ{v.x, v.y};
}

} // namespace pomdog::math
