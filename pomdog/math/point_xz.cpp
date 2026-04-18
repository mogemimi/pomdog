// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/point_xz.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector_xz.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

PointXZ::PointXZ() noexcept = default;

PointXZ::PointXZ(i32 xIn, i32 zIn) noexcept
    : x(xIn)
    , z(zIn)
{
}

PointXZ& PointXZ::operator+=(const PointXZ& other) noexcept
{
    x += other.x;
    z += other.z;
    return *this;
}

PointXZ& PointXZ::operator-=(const PointXZ& other) noexcept
{
    x -= other.x;
    z -= other.z;
    return *this;
}

PointXZ& PointXZ::operator*=(i32 factor) noexcept
{
    x *= factor;
    z *= factor;
    return *this;
}

PointXZ& PointXZ::operator/=(i32 factor) noexcept
{
    POMDOG_ASSERT(factor != 0);
    x /= factor;
    z /= factor;
    return *this;
}

PointXZ PointXZ::operator+() const noexcept
{
    return *this;
}

PointXZ PointXZ::operator-() const noexcept
{
    return PointXZ{-x, -z};
}

PointXZ PointXZ::operator+(const PointXZ& other) const noexcept
{
    return PointXZ{
        x + other.x,
        z + other.z,
    };
}

PointXZ PointXZ::operator-(const PointXZ& other) const noexcept
{
    return PointXZ{
        x - other.x,
        z - other.z,
    };
}

PointXZ PointXZ::operator*(const PointXZ& other) const noexcept
{
    return PointXZ{
        x * other.x,
        z * other.z,
    };
}

PointXZ PointXZ::operator/(const PointXZ& other) const noexcept
{
    return PointXZ{
        x / other.x,
        z / other.z,
    };
}

PointXZ PointXZ::operator*(i32 factor) const noexcept
{
    return PointXZ{
        x * factor,
        z * factor,
    };
}

PointXZ PointXZ::operator/(i32 factor) const noexcept
{
    POMDOG_ASSERT(factor != 0);
    return PointXZ{
        x / factor,
        z / factor,
    };
}

bool PointXZ::operator==(const PointXZ& other) const noexcept
{
    return ((x == other.x) && (z == other.z));
}

bool PointXZ::operator!=(const PointXZ& other) const noexcept
{
    return ((x != other.x) || (z != other.z));
}

[[nodiscard]] PointXZ
operator*(i32 factor, const PointXZ& v) noexcept
{
    return PointXZ{
        factor * v.x,
        factor * v.z,
    };
}

} // namespace pomdog

namespace pomdog::math {

PointXZ
abs(PointXZ v) noexcept
{
    return PointXZ{
        std::abs(v.x),
        std::abs(v.z),
    };
}

PointXZ
min(PointXZ a, PointXZ b) noexcept
{
    return PointXZ{
        std::min(a.x, b.x),
        std::min(a.z, b.z),
    };
}

PointXZ
max(PointXZ a, PointXZ b) noexcept
{
    return PointXZ{
        std::max(a.x, b.x),
        std::max(a.z, b.z),
    };
}

PointXZ
clamp(PointXZ source, PointXZ min, PointXZ max) noexcept
{
    return PointXZ{
        std::clamp(source.x, min.x, max.x),
        std::clamp(source.z, min.z, max.z),
    };
}

Vector2
toVector2(PointXZ p) noexcept
{
    return Vector2{
        static_cast<f32>(p.x),
        static_cast<f32>(p.z),
    };
}

VectorXZ
toVectorXZ(PointXZ p) noexcept
{
    return VectorXZ{
        static_cast<f32>(p.x),
        static_cast<f32>(p.z),
    };
}

} // namespace pomdog::math
