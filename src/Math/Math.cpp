// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/Math.hpp"
#include "Pomdog/Math/Point2D.hpp"
#include "Pomdog/Math/Point3D.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include <cmath>

namespace Pomdog::Math {

Point2D ToPoint2D(const Vector2& vec) noexcept
{
    return Point2D{
        static_cast<std::int32_t>(vec.X),
        static_cast<std::int32_t>(vec.Y)
    };
}

Vector2 ToVector2(const Point2D& point) noexcept
{
    return Vector2{
        static_cast<float>(point.X),
        static_cast<float>(point.Y)
    };
}

Point3D ToPoint3D(const Vector3& vec) noexcept
{
    return Point3D{
        static_cast<std::int32_t>(vec.X),
        static_cast<std::int32_t>(vec.Y),
        static_cast<std::int32_t>(vec.Z)
    };
}

Vector3 ToVector3(const Point3D& point) noexcept
{
    return Vector3{
        static_cast<float>(point.X),
        static_cast<float>(point.Y),
        static_cast<float>(point.Z)
    };
}

} // namespace Pomdog::Math
