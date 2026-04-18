// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/f32_color.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

static_assert(std::is_trivially_copyable_v<F32Color>);
static_assert(std::is_standard_layout_v<F32Color>);

F32Color::F32Color() noexcept = default;

F32Color::F32Color(f32 rIn, f32 gIn, f32 bIn, f32 aIn) noexcept
    : r(rIn)
    , g(gIn)
    , b(bIn)
    , a(aIn)
{
}

F32Color& F32Color::operator+=(const F32Color& v) noexcept
{
    r += v.r;
    g += v.g;
    b += v.b;
    a += v.a;
    return *this;
}

F32Color& F32Color::operator-=(const F32Color& v) noexcept
{
    r -= v.r;
    g -= v.g;
    b -= v.b;
    a -= v.a;
    return *this;
}

F32Color& F32Color::operator*=(f32 factor) noexcept
{
    r *= factor;
    g *= factor;
    b *= factor;
    a *= factor;
    return *this;
}

F32Color& F32Color::operator/=(f32 factor) noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    r /= factor;
    g /= factor;
    b /= factor;
    a /= factor;
    return *this;
}

F32Color F32Color::operator+() const noexcept
{
    return *this;
}

F32Color F32Color::operator-() const noexcept
{
    return F32Color{-r, -g, -b, -a};
}

F32Color F32Color::operator+(const F32Color& v) const noexcept
{
    return F32Color{
        r + v.r,
        g + v.g,
        b + v.b,
        a + v.a,
    };
}

F32Color F32Color::operator-(const F32Color& v) const noexcept
{
    return F32Color{
        r - v.r,
        g - v.g,
        b - v.b,
        a - v.a,
    };
}

F32Color F32Color::operator*(const F32Color& v) const noexcept
{
    return F32Color{
        r * v.r,
        g * v.g,
        b * v.b,
        a * v.a,
    };
}

F32Color F32Color::operator/(const F32Color& v) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(v.r) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(v.g) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(v.b) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(v.a) != FP_ZERO);
    return F32Color{
        r / v.r,
        g / v.g,
        b / v.b,
        a / v.a,
    };
}

F32Color F32Color::operator*(f32 factor) const noexcept
{
    return F32Color{
        r * factor,
        g * factor,
        b * factor,
        a * factor,
    };
}

F32Color F32Color::operator/(f32 factor) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    return F32Color{
        r / factor,
        g / factor,
        b / factor,
        a / factor,
    };
}

bool F32Color::operator==(const F32Color& other) const noexcept
{
    return (r == other.r) && (g == other.g) && (b == other.b) && (a == other.a);
}

bool F32Color::operator!=(const F32Color& other) const noexcept
{
    return (r != other.r) || (g != other.g) || (b != other.b) || (a != other.a);
}

const f32* F32Color::data() const noexcept
{
    return &r;
}

} // namespace pomdog

namespace pomdog::math {

[[nodiscard]] F32Color
abs(F32Color v) noexcept
{
    return F32Color{
        std::abs(v.r),
        std::abs(v.g),
        std::abs(v.b),
        std::abs(v.a),
    };
}

} // namespace pomdog::math
