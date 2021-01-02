// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/Math.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Math/Point2D.hpp"
#include "Pomdog/Math/Point3D.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Vector3.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <cstdlib>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Math {

Point2D ToPoint2D(const Vector2& vec) noexcept
{
    return Point2D{
        static_cast<std::int32_t>(vec.X),
        static_cast<std::int32_t>(vec.Y),
    };
}

Vector2 ToVector2(const Point2D& point) noexcept
{
    return Vector2{
        static_cast<float>(point.X),
        static_cast<float>(point.Y),
    };
}

Point3D ToPoint3D(const Vector3& vec) noexcept
{
    return Point3D{
        static_cast<std::int32_t>(vec.X),
        static_cast<std::int32_t>(vec.Y),
        static_cast<std::int32_t>(vec.Z),
    };
}

Vector3 ToVector3(const Point3D& point) noexcept
{
    return Vector3{
        static_cast<float>(point.X),
        static_cast<float>(point.Y),
        static_cast<float>(point.Z),
    };
}

Point2D Abs(const Point2D& point) noexcept
{
    return Point2D{
        std::abs(point.X),
        std::abs(point.Y),
    };
}

Point3D Abs(const Point3D& point) noexcept
{
    return Point3D{
        std::abs(point.X),
        std::abs(point.Y),
        std::abs(point.Z),
    };
}

} // namespace Pomdog::Math
