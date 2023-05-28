// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/viewport.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/math/vector3.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

Viewport::Viewport(const Rectangle& bounds) noexcept
    : topLeftX(bounds.x)
    , topLeftY(bounds.y)
    , width(bounds.width)
    , height(bounds.height)
    , minDepth(0.0f)
    , maxDepth(1.0f)
{
}

Viewport::Viewport(int xIn, int yIn, int widthIn, int heightIn) noexcept
    : topLeftX(xIn)
    , topLeftY(yIn)
    , width(widthIn)
    , height(heightIn)
    , minDepth(0.0f)
    , maxDepth(1.0f)
{
}

Viewport::Viewport(int xIn, int yIn, int widthIn, int heightIn, float minDepthIn, float maxDepthIn) noexcept
    : topLeftX(xIn)
    , topLeftY(yIn)
    , width(widthIn)
    , height(heightIn)
    , minDepth(minDepthIn)
    , maxDepth(maxDepthIn)
{
}

Vector3 Viewport::project(
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

    result.x = (result.x + 1.0f) * 0.5f * static_cast<float>(width) + static_cast<float>(topLeftX);
    result.y = (-result.y + 1.0f) * 0.5f * static_cast<float>(height) + static_cast<float>(topLeftY);
    result.z = result.z * (maxDepth - minDepth) + minDepth;
    return result;
}

Vector3 Viewport::unproject(
    const Vector3& source,
    const Matrix4x4& worldViewProjection) const
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);
    POMDOG_ASSERT((maxDepth - minDepth) != 0.0f);

    Vector3 vec;
    vec.x = ((source.x - static_cast<float>(topLeftX)) / static_cast<float>(width) * 2.0f) - 1.0f;
    vec.y = -(((source.y - static_cast<float>(topLeftY)) / static_cast<float>(height) * 2.0f) - 1.0f);
    vec.z = (source.z - minDepth) / (maxDepth - minDepth);

    const auto invertWVP = math::invert(worldViewProjection);
    const auto divisor = vec.x * invertWVP(0, 3) +
                         vec.y * invertWVP(1, 3) +
                         vec.z * invertWVP(2, 3) +
                         invertWVP(3, 3);

    auto result = math::transform(vec, invertWVP);
    if (std::abs(divisor) > std::numeric_limits<float>::epsilon()) {
        result = result / divisor;
    }
    return result;
}

Rectangle Viewport::getBounds() const noexcept
{
    return Rectangle{topLeftX, topLeftY, width, height};
}

float Viewport::getAspectRatio() const noexcept
{
    POMDOG_ASSERT(height != 0);
    if (height != 0) {
        return static_cast<float>(width) / height;
    }
    return 0.0f;
}

} // namespace pomdog::gpu
