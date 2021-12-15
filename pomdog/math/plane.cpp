// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/plane.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/math/bounding_box.hpp"
#include "pomdog/math/bounding_frustum.hpp"
#include "pomdog/math/bounding_sphere.hpp"
#include "pomdog/math/matrix4x4.hpp"
#include "pomdog/math/plane_intersection_type.hpp"
#include "pomdog/math/vector4.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

Plane::Plane() noexcept
    : Normal(0.0f, 0.0f, 0.0f)
    , Distance(0.0f)
{
}

Plane::Plane(const Vector3& normalIn, float distanceIn) noexcept
    : Normal(normalIn)
    , Distance(distanceIn)
{
}

Plane::Plane(const Vector3& point0, const Vector3& point1, const Vector3& point2) noexcept
{
    // NOTE: Left-handed coordinate system (not right-handed one)
    const auto vector1 = point1 - point0;
    const auto vector2 = point2 - point0;
    Normal = math::Normalize(math::Cross(vector1, vector2));
    Distance = -math::Dot(Normal, point0);
}

void Plane::Normalize() noexcept
{
    const auto length = math::Length(this->Normal);

    if (length >= std::numeric_limits<float>::epsilon()) {
        const auto inverseLength = 1.0f / length;
        Normal *= inverseLength;
        Distance *= inverseLength;
    }
}

Plane Plane::Normalize(const Plane& plane) noexcept
{
    auto result = plane;
    result.Normalize();
    return result;
}

float Plane::Dot(const Vector4& vec) const noexcept
{
    return this->Normal.X * vec.X +
           this->Normal.Y * vec.Y +
           this->Normal.Z * vec.Z +
           this->Distance * vec.W;
}

float Plane::DotCoordinate(const Vector3& vec) const noexcept
{
    return Normal.X * vec.X + Normal.Y * vec.Y + Normal.Z * vec.Z + Distance;
}

float Plane::DotNormal(const Vector3& vec) const noexcept
{
    return math::Dot(this->Normal, vec);
}

float Plane::GetDistanceToPoint(const Vector3& point) const noexcept
{
    return this->DotCoordinate(point);
}

PlaneIntersectionType Plane::Intersects(const Vector3& point) const noexcept
{
    const auto distance = this->DotCoordinate(point);

    if (distance > 0.0f) {
        return PlaneIntersectionType::Front;
    }
    if (distance < 0.0f) {
        return PlaneIntersectionType::Back;
    }
    return PlaneIntersectionType::Intersecting;
}

PlaneIntersectionType Plane::Intersects(const BoundingBox& box) const noexcept
{
    // NOTE: See also
    // http://zach.in.tu-clausthal.de/teaching/cg_literatur/lighthouse3d_view_frustum_culling/index.html

    Vector3 positiveVertex{box.Min.X, box.Min.Y, box.Min.Z};
    Vector3 negativeVertex{box.Max.X, box.Max.Y, box.Max.Z};

    if (Normal.X >= 0.0f) {
        positiveVertex.X = box.Max.X;
        negativeVertex.X = box.Min.X;
    }
    if (Normal.Y >= 0.0f) {
        positiveVertex.Y = box.Max.Y;
        negativeVertex.Y = box.Min.Y;
    }
    if (Normal.Z >= 0.0f) {
        positiveVertex.Z = box.Max.Z;
        negativeVertex.Z = box.Min.Z;
    }

    if (this->DotCoordinate(negativeVertex) > 0.0f) {
        return PlaneIntersectionType::Front;
    }
    if (this->DotCoordinate(positiveVertex) < 0.0f) {
        return PlaneIntersectionType::Back;
    }
    return PlaneIntersectionType::Intersecting;
}

PlaneIntersectionType Plane::Intersects(const BoundingFrustum& frustum) const
{
    return frustum.Intersects(*this);
}

PlaneIntersectionType Plane::Intersects(const BoundingSphere& sphere) const noexcept
{
    const auto distance = this->DotCoordinate(sphere.Center);

    if (distance > sphere.Radius) {
        return PlaneIntersectionType::Front;
    }
    if (distance < -sphere.Radius) {
        return PlaneIntersectionType::Back;
    }
    return PlaneIntersectionType::Intersecting;
}

Plane Plane::Transform(const Plane& plane, const Matrix4x4& matrix)
{
    const auto transformMatrix = Matrix4x4::Invert(matrix);
    const auto vector = Vector4{plane.Normal, plane.Distance};
    const auto transformedVector = math::Transform(vector, transformMatrix);

    Plane result;
    result.Normal.X = transformedVector.X;
    result.Normal.Y = transformedVector.Y;
    result.Normal.Z = transformedVector.Z;
    result.Distance = transformedVector.W;
    return result;
}

Plane Plane::CreateFromPointNormal(const Vector3& point, const Vector3& normal)
{
    return Plane(normal, -math::Dot(normal, point));
}

} // namespace pomdog
