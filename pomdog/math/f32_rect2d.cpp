// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/f32_rect2d.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

f32 F32Rect2D::minX() const noexcept
{
    return x;
}

f32 F32Rect2D::maxX() const noexcept
{
    return x + width;
}

f32 F32Rect2D::minY() const noexcept
{
    return y;
}

f32 F32Rect2D::maxY() const noexcept
{
    return y + height;
}

Vector2 F32Rect2D::center() const noexcept
{
    return Vector2{x + width * 0.5f, y + height * 0.5f};
}

Vector2 F32Rect2D::location() const noexcept
{
    return Vector2{x, y};
}

bool F32Rect2D::contains(f32 pointX, f32 pointY) const noexcept
{
    return (pointX >= x) &&
           (pointX <= (x + width)) &&
           (pointY >= y) &&
           (pointY <= (y + height));
}

bool F32Rect2D::contains(const Vector2& point) const noexcept
{
    return contains(point.x, point.y);
}

F32Rect2D
F32Rect2D::createFromMinMax(Vector2 min, Vector2 max)
{
    F32Rect2D rect;
    rect.x = min.x;
    rect.y = min.y;
    rect.width = max.x - min.x;
    rect.height = max.y - min.y;
    return rect;
}

} // namespace pomdog

namespace pomdog::math {

[[nodiscard]] bool
approxEqual(const F32Rect2D& a, const F32Rect2D& b, f32 tolerance) noexcept
{
    return approxEqual(a.x, b.x, tolerance) &&
           approxEqual(a.y, b.y, tolerance) &&
           approxEqual(a.width, b.width, tolerance) &&
           approxEqual(a.height, b.height, tolerance);
}

[[nodiscard]] bool
approxEqual(const F32Rect2D& a, const F32Rect2D& b) noexcept
{
    constexpr f32 tolerance = std::numeric_limits<f32>::epsilon();
    return approxEqual(a, b, tolerance);
}

} // namespace pomdog::math
