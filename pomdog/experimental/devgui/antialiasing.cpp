// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/antialiasing.h"
#include "pomdog/math/color.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <cmath>
#include <span>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui::detail {
namespace {

[[nodiscard]] bool
appendContourPoint(std::span<Vector2> points, std::size_t& count, const Vector2& point) noexcept
{
    if ((count > 0) && (math::distanceSquared(points[count - 1], point) <= 0.00000001f)) {
        return true;
    }
    if (count >= points.size()) {
        return false;
    }
    points[count++] = point;
    return true;
}

} // namespace

/// Clips the inset against each edge, removing corners whose edges disappear.
[[nodiscard]] std::size_t
clipConvexInset(
    std::span<const Vector2> points,
    std::span<const Vector2> normals,
    f32 inset,
    std::span<Vector2> output) noexcept
{
    std::array<Vector2, clippedContourCapacity> scratch;
    auto source = output;
    auto destination = std::span<Vector2>{scratch};
    std::copy(points.begin(), points.end(), source.begin());
    auto count = points.size();
    for (std::size_t edge = 0; (edge < points.size()) && (count > 0); ++edge) {
        auto previous = source[count - 1];
        auto previousDistance = math::dot(previous - points[edge], normals[edge]) + inset;
        auto previousInside = previousDistance <= 0.0f;
        std::size_t nextCount = 0;
        for (std::size_t i = 0; i < count; ++i) {
            const auto current = source[i];
            const auto distance = math::dot(current - points[edge], normals[edge]) + inset;
            const auto inside = distance <= 0.0f;
            if (inside != previousInside) {
                const auto t = previousDistance / (previousDistance - distance);
                if (!appendContourPoint(destination, nextCount, previous + (current - previous) * t)) {
                    return 0;
                }
            }
            if (inside && !appendContourPoint(destination, nextCount, current)) {
                return 0;
            }
            previous = current;
            previousDistance = distance;
            previousInside = inside;
        }
        if ((nextCount > 1) && (math::distanceSquared(destination[0], destination[nextCount - 1]) <= 0.00000001f)) {
            --nextCount;
        }
        count = nextCount;
        std::swap(source, destination);
    }
    if (source.data() != output.data()) {
        std::copy(source.first(count).begin(), source.first(count).end(), output.begin());
    }
    return count;
}

/// Returns a counterclockwise convex boundary for the expanded contour.
[[nodiscard]] std::size_t
makeContourHull(std::span<Vector2> points, std::span<Vector2> hull) noexcept
{
    std::sort(points.begin(), points.end(), [](const Vector2& a, const Vector2& b) {
        return (a.x < b.x) || ((a.x == b.x) && (a.y < b.y));
    });
    std::size_t count = 0;
    for (const auto& point : points) {
        while ((count >= 2) && (math::cross(hull[count - 1] - hull[count - 2], point - hull[count - 1]) <= 0.0f)) {
            --count;
        }
        hull[count++] = point;
    }
    const auto upperBegin = count + 1;
    for (auto i = points.size() - 1; i > 0; --i) {
        const auto& point = points[i - 1];
        while ((count >= upperBegin) && (math::cross(hull[count - 1] - hull[count - 2], point - hull[count - 1]) <= 0.0f)) {
            --count;
        }
        hull[count++] = point;
    }
    return count - 1;
}

/// Returns the outward offset at a corner shared by two unit normals.
[[nodiscard]] Vector2
contourMiter(const Vector2& previous, const Vector2& next) noexcept
{
    const auto sum = previous + next;
    const auto denominator = std::max(1.0f + math::dot(previous, next), 0.0001f);
    auto offset = sum / denominator;
    const auto length = math::length(offset);
    if (length > 4.0f) {
        offset *= 4.0f / length;
    }
    return offset;
}

/// Returns the quarter-circle segments for a bounded physical-pixel chord error.
f32 snapToPixels(f32 value, f32 pixelRatio) noexcept
{
    return std::round(value * pixelRatio) / pixelRatio;
}

PixelSpan snapSpanToPixels(f32 start, f32 length, f32 pixelRatio) noexcept
{
    // NOTE: A thin span, such as a border, keeps its rounded width wherever it
    // lies, so that it does not change between one and two pixels as the
    // window resizes. A wide span rounds both ends, so that neighbors meet.
    constexpr f32 thinLength = 4.0f;
    const auto first = std::round(start * pixelRatio);
    const auto physicalLength = length * pixelRatio;
    auto last = std::round((start + length) * pixelRatio);
    if ((length > 0.0f) && (physicalLength < thinLength)) {
        last = first + std::max(1.0f, std::round(physicalLength));
    }
    return PixelSpan{first / pixelRatio, (last - first) / pixelRatio};
}

f32 snapThicknessToPixels(f32 thickness, f32 pixelRatio) noexcept
{
    return std::max(1.0f, std::round(thickness * pixelRatio)) / pixelRatio;
}

[[nodiscard]] i32
quarterCircleSegments(f32 radius, f32 pixelRatio) noexcept
{
    const auto physicalRadius = std::max(radius * pixelRatio, 0.125f);
    const auto step = 2.0f * std::acos(std::clamp(1.0f - 0.125f / physicalRadius, 0.0f, 1.0f));
    if (step <= 0.000001f) {
        return 32;
    }
    return std::clamp(static_cast<i32>(std::ceil(math::PiOver2<f32> / step)), 3, 32);
}

/// Keeps corner samples paired when an inset radius or dimension collapses.
[[nodiscard]] Vector2
roundedContourPoint(f32 left, f32 bottom, f32 width, f32 height, f32 radius, i32 corner, f32 angle, f32 offset) noexcept
{
    const auto centerX = left + width * 0.5f;
    const auto centerY = bottom + height * 0.5f;
    const auto halfWidth = std::max(0.0f, width * 0.5f + offset);
    const auto halfHeight = std::max(0.0f, height * 0.5f + offset);
    const auto r = std::clamp(radius + offset, 0.0f, std::min(halfWidth, halfHeight));
    auto center = Vector2{centerX, centerY};
    if ((corner == 0) || (corner == 3)) {
        center.x += halfWidth - r;
    }
    else {
        center.x -= halfWidth - r;
    }
    if (corner < 2) {
        center.y += halfHeight - r;
    }
    else {
        center.y -= halfHeight - r;
    }
    return Vector2{center.x + r * std::cos(angle), center.y + r * std::sin(angle)};
}

} // namespace pomdog::devgui::detail
