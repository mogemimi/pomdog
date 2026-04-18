// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/raycast.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/f32_rect2d.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/ray.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector3.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::math {

POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(5045) // NOTE: Spectre mitigation
[[nodiscard]] Vector3
project(
    const F32Rect2D& viewport,
    f32 minDepth,
    f32 maxDepth,
    const Vector3& source,
    const Matrix4x4& worldViewProjection) noexcept
{
    POMDOG_ASSERT(viewport.width > 0.0f);
    POMDOG_ASSERT(viewport.height > 0.0f);
    POMDOG_ASSERT(minDepth > 0.0f);
    POMDOG_ASSERT(maxDepth > 0.0f);
    POMDOG_ASSERT(minDepth < maxDepth);

    const auto divisor = source.x * worldViewProjection(0, 3) +
                         source.y * worldViewProjection(1, 3) +
                         source.z * worldViewProjection(2, 3) +
                         worldViewProjection(3, 3);

    auto result = math::transform(source, worldViewProjection);

    if (std::abs(divisor) > std::numeric_limits<f32>::epsilon()) {
        result = result / divisor;
    }

    result.x = (result.x + 1.0f) * 0.5f * viewport.width + viewport.x;
    result.y = (-result.y + 1.0f) * 0.5f * viewport.height + viewport.y;
    result.z = result.z * (maxDepth - minDepth) + minDepth;
    return result;
}
POMDOG_MSVC_SUPPRESS_WARNING_POP

POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(5045) // NOTE: Spectre mitigation
[[nodiscard]] Vector3
unproject(
    const F32Rect2D& viewport,
    f32 minDepth,
    f32 maxDepth,
    const Vector3& source,
    const Matrix4x4& inverseWorldViewProjection) noexcept
{
    POMDOG_ASSERT(viewport.width > 0.0f);
    POMDOG_ASSERT(viewport.height > 0.0f);
    POMDOG_ASSERT(minDepth > 0.0f);
    POMDOG_ASSERT(maxDepth > 0.0f);
    POMDOG_ASSERT(minDepth < maxDepth);

    Vector3 vec;
    vec.x = ((source.x - viewport.x) / viewport.width * 2.0f) - 1.0f;
    vec.y = -(((source.y - viewport.y) / viewport.height * 2.0f) - 1.0f);
    vec.z = (source.z - minDepth) / (maxDepth - minDepth);

    const auto divisor = vec.x * inverseWorldViewProjection(0, 3) +
                         vec.y * inverseWorldViewProjection(1, 3) +
                         vec.z * inverseWorldViewProjection(2, 3) +
                         inverseWorldViewProjection(3, 3);

    auto result = math::transform(vec, inverseWorldViewProjection);

    if (std::abs(divisor) > std::numeric_limits<f32>::epsilon()) {
        result = result / divisor;
    }
    return result;
}
POMDOG_MSVC_SUPPRESS_WARNING_POP

[[nodiscard]] Ray
screenPointToRay(
    const Vector2& screenPoint,
    const Vector3& cameraPosition,
    const F32Rect2D& viewport,
    f32 minDepth,
    f32 maxDepth,
    const Matrix4x4& inverseViewProjection,
    bool isOrthoProjection)
{
    const auto screenPointVector = Vector3(screenPoint.x, screenPoint.y, 1.0f);
    const auto worldPoint = unproject(viewport, minDepth, maxDepth, screenPointVector, inverseViewProjection);

    if (isOrthoProjection) {
        const auto cameraPositionInScreen = Vector3(screenPoint.x, screenPoint.y, -1.0f);
        const auto cameraPositionInWorld = unproject(viewport, minDepth, maxDepth, cameraPositionInScreen, inverseViewProjection);
        return Ray{cameraPositionInWorld, math::normalize(worldPoint - cameraPositionInWorld)};
    }

    return Ray{cameraPosition, math::normalize(worldPoint - cameraPosition)};
}

} // namespace pomdog::math
