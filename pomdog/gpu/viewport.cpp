// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/viewport.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/math/vector3.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

Viewport::Viewport(const Rect2D& bounds) noexcept
    : topLeftX(bounds.x)
    , topLeftY(bounds.y)
    , width(bounds.width)
    , height(bounds.height)
    , minDepth(0.0f)
    , maxDepth(1.0f)
{
}

Viewport::Viewport(i32 xIn, i32 yIn, i32 widthIn, i32 heightIn) noexcept
    : topLeftX(xIn)
    , topLeftY(yIn)
    , width(widthIn)
    , height(heightIn)
    , minDepth(0.0f)
    , maxDepth(1.0f)
{
}

Viewport::Viewport(i32 xIn, i32 yIn, i32 widthIn, i32 heightIn, f32 minDepthIn, f32 maxDepthIn) noexcept
    : topLeftX(xIn)
    , topLeftY(yIn)
    , width(widthIn)
    , height(heightIn)
    , minDepth(minDepthIn)
    , maxDepth(maxDepthIn)
{
}

Vector3
Viewport::project(
    const Vector3& source,
    const Matrix4x4& worldViewProjection) const
{
    const auto divisor = source.x * worldViewProjection(0, 3) +
                         source.y * worldViewProjection(1, 3) +
                         source.z * worldViewProjection(2, 3) +
                         worldViewProjection(3, 3);

    auto result = math::transform(source, worldViewProjection);

    if (std::abs(divisor) > std::numeric_limits<float>::epsilon()) {
        result = result / divisor;
    }

    result.x = (result.x + 1.0f) * 0.5f * static_cast<f32>(width) + static_cast<f32>(topLeftX);
    result.y = (-result.y + 1.0f) * 0.5f * static_cast<f32>(height) + static_cast<f32>(topLeftY);
    result.z = result.z * (maxDepth - minDepth) + minDepth;
    return result;
}

Vector3
Viewport::unproject(
    const Vector3& source,
    const Matrix4x4& worldViewProjection) const
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);
    POMDOG_ASSERT((maxDepth - minDepth) != 0.0f);

    Vector3 vec;
    vec.x = ((source.x - static_cast<f32>(topLeftX)) / static_cast<f32>(width) * 2.0f) - 1.0f;
    vec.y = -(((source.y - static_cast<f32>(topLeftY)) / static_cast<f32>(height) * 2.0f) - 1.0f);
    vec.z = (source.z - minDepth) / (maxDepth - minDepth);

    const auto invertWVP = math::invert(worldViewProjection);
    const auto divisor = vec.x * invertWVP(0, 3) +
                         vec.y * invertWVP(1, 3) +
                         vec.z * invertWVP(2, 3) +
                         invertWVP(3, 3);

    auto result = math::transform(vec, invertWVP);
    if (std::abs(divisor) > std::numeric_limits<f32>::epsilon()) {
        result = result / divisor;
    }
    return result;
}

Rect2D
Viewport::getBounds() const noexcept
{
    return Rect2D{topLeftX, topLeftY, width, height};
}

f32 Viewport::getAspectRatio() const noexcept
{
    POMDOG_ASSERT(height != 0);
    if (height != 0) {
        return static_cast<f32>(width) / static_cast<f32>(height);
    }
    return 0.0f;
}

} // namespace pomdog::gpu
