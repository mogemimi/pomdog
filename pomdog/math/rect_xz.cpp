// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/rect_xz.h"
#include "pomdog/math/point2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

bool RectXZ::operator==(const RectXZ& other) const noexcept
{
    return (min == other.min) && (max == other.max);
}

bool RectXZ::operator!=(const RectXZ& other) const noexcept
{
    return (min != other.min) || (max != other.max);
}

i32 RectXZ::dx() const noexcept
{
    return max.x - min.x;
}

i32 RectXZ::dz() const noexcept
{
    return max.z - min.z;
}

i32 RectXZ::minX() const noexcept
{
    return min.x;
}

i32 RectXZ::maxX() const noexcept
{
    return max.x;
}

i32 RectXZ::minZ() const noexcept
{
    return min.z;
}

i32 RectXZ::maxZ() const noexcept
{
    return max.z;
}

bool RectXZ::contains(i32 px, i32 pz) const noexcept
{
    return (px >= min.x) && (px <= max.x) &&
           (pz >= min.z) && (pz <= max.z);
}

bool RectXZ::contains(const Point2D& point) const noexcept
{
    return contains(point.x, point.y);
}

bool RectXZ::contains(const PointXZ& point) const noexcept
{
    return contains(point.x, point.z);
}

bool RectXZ::intersects(RectXZ bounds) const noexcept
{
    return (max.x >= bounds.min.x && min.x <= bounds.max.x) &&
           (max.z >= bounds.min.z && min.z <= bounds.max.z);
}

std::optional<RectXZ>
RectXZ::overlap(RectXZ bounds) const noexcept
{
    if (!intersects(bounds)) {
        return std::nullopt;
    }
    RectXZ result;
    result.min = pomdog::math::max(min, bounds.min);
    result.max = pomdog::math::min(max, bounds.max);
    return result;
}

} // namespace pomdog
