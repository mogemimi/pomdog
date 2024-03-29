// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/plane.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/bounding_box.h"
#include "pomdog/math/bounding_frustum.h"
#include "pomdog/math/bounding_sphere.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/plane_intersection_type.h"
#include "pomdog/math/vector4.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

Plane::Plane() noexcept
    : normal(0.0f, 0.0f, 0.0f)
    , distance(0.0f)
{
}

Plane::Plane(const Vector3& normalIn, float distanceIn) noexcept
    : normal(normalIn)
    , distance(distanceIn)
{
}

Plane::Plane(const Vector3& point0, const Vector3& point1, const Vector3& point2) noexcept
{
    // NOTE: Left-handed coordinate system (not right-handed one)
    const auto vector1 = point1 - point0;
    const auto vector2 = point2 - point0;
    normal = math::normalize(math::cross(vector1, vector2));
    distance = -math::dot(normal, point0);
}

void Plane::normalize() noexcept
{
    const auto length = math::length(normal);

    if (length >= std::numeric_limits<float>::epsilon()) {
        const auto inverseLength = 1.0f / length;
        normal *= inverseLength;
        distance *= inverseLength;
    }
}

Plane Plane::normalize(const Plane& plane) noexcept
{
    auto result = plane;
    result.normalize();
    return result;
}

float Plane::dot(const Vector4& vec) const noexcept
{
    return normal.x * vec.x +
           normal.y * vec.y +
           normal.z * vec.z +
           distance * vec.w;
}

float Plane::dotCoordinate(const Vector3& vec) const noexcept
{
    return normal.x * vec.x + normal.y * vec.y + normal.z * vec.z + distance;
}

float Plane::dotNormal(const Vector3& vec) const noexcept
{
    return math::dot(normal, vec);
}

float Plane::getDistanceToPoint(const Vector3& point) const noexcept
{
    return dotCoordinate(point);
}

PlaneIntersectionType
Plane::intersects(const Vector3& point) const noexcept
{
    const auto dotProduct = dotCoordinate(point);

    if (dotProduct > 0.0f) {
        return PlaneIntersectionType::Front;
    }
    if (dotProduct < 0.0f) {
        return PlaneIntersectionType::Back;
    }
    return PlaneIntersectionType::Intersecting;
}

PlaneIntersectionType
Plane::intersects(const BoundingBox& box) const noexcept
{
    Vector3 positiveVertex{box.min.x, box.min.y, box.min.z};
    Vector3 negativeVertex{box.max.x, box.max.y, box.max.z};

    if (normal.x >= 0.0f) {
        positiveVertex.x = box.max.x;
        negativeVertex.x = box.min.x;
    }
    if (normal.y >= 0.0f) {
        positiveVertex.y = box.max.y;
        negativeVertex.y = box.min.y;
    }
    if (normal.z >= 0.0f) {
        positiveVertex.z = box.max.z;
        negativeVertex.z = box.min.z;
    }

    if (dotCoordinate(negativeVertex) > 0.0f) {
        return PlaneIntersectionType::Front;
    }
    if (dotCoordinate(positiveVertex) < 0.0f) {
        return PlaneIntersectionType::Back;
    }
    return PlaneIntersectionType::Intersecting;
}

PlaneIntersectionType
Plane::intersects(const BoundingFrustum& frustum) const
{
    return frustum.intersects(*this);
}

PlaneIntersectionType
Plane::intersects(const BoundingSphere& sphere) const noexcept
{
    const auto dotProduct = dotCoordinate(sphere.center);

    if (dotProduct > sphere.radius) {
        return PlaneIntersectionType::Front;
    }
    if (dotProduct < -sphere.radius) {
        return PlaneIntersectionType::Back;
    }
    return PlaneIntersectionType::Intersecting;
}

Plane Plane::transform(const Plane& plane, const Matrix4x4& matrix)
{
    const auto transformMatrix = math::invert(matrix);
    const auto vector = Vector4{plane.normal, plane.distance};
    const auto transformedVector = math::transform(vector, transformMatrix);

    Plane result;
    result.normal.x = transformedVector.x;
    result.normal.y = transformedVector.y;
    result.normal.z = transformedVector.z;
    result.distance = transformedVector.w;
    return result;
}

Plane Plane::createFromPointNormal(const Vector3& point, const Vector3& normal)
{
    return Plane(normal, -math::dot(normal, point));
}

} // namespace pomdog
