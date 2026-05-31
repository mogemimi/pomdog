// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/pixel_ratio_conversion.h"
#include "pomdog/math/point2d.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/math/vector2.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::math {
namespace {

[[nodiscard]] i32
scaleAndRound(i32 value, f32 factor) noexcept
{
    return static_cast<i32>(std::lround(static_cast<f32>(value) * factor));
}

} // namespace

Point2D
toPhysicalPixels(const Point2D& logical, f32 pixelRatio) noexcept
{
    POMDOG_ASSERT(pixelRatio > 0.0f);
    return Point2D{
        scaleAndRound(logical.x, pixelRatio),
        scaleAndRound(logical.y, pixelRatio),
    };
}

Point2D
toLogicalPixels(const Point2D& physical, f32 pixelRatio) noexcept
{
    POMDOG_ASSERT(pixelRatio > 0.0f);
    const f32 inv = 1.0f / pixelRatio;
    return Point2D{
        scaleAndRound(physical.x, inv),
        scaleAndRound(physical.y, inv),
    };
}

Vector2
toPhysicalPixels(const Vector2& logical, f32 pixelRatio) noexcept
{
    POMDOG_ASSERT(pixelRatio > 0.0f);
    return Vector2{
        logical.x * pixelRatio,
        logical.y * pixelRatio,
    };
}

Vector2
toLogicalPixels(const Vector2& physical, f32 pixelRatio) noexcept
{
    POMDOG_ASSERT(pixelRatio > 0.0f);
    const f32 inv = 1.0f / pixelRatio;
    return Vector2{
        physical.x * inv,
        physical.y * inv,
    };
}

Rect2D
toPhysicalPixels(const Rect2D& logical, f32 pixelRatio) noexcept
{
    POMDOG_ASSERT(pixelRatio > 0.0f);
    return Rect2D{
        scaleAndRound(logical.x, pixelRatio),
        scaleAndRound(logical.y, pixelRatio),
        scaleAndRound(logical.width, pixelRatio),
        scaleAndRound(logical.height, pixelRatio),
    };
}

Rect2D
toLogicalPixels(const Rect2D& physical, f32 pixelRatio) noexcept
{
    POMDOG_ASSERT(pixelRatio > 0.0f);
    const f32 inv = 1.0f / pixelRatio;
    return Rect2D{
        scaleAndRound(physical.x, inv),
        scaleAndRound(physical.y, inv),
        scaleAndRound(physical.width, inv),
        scaleAndRound(physical.height, inv),
    };
}

} // namespace pomdog::math
