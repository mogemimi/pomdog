// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
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

constexpr std::size_t contourCapacity = 132;
constexpr std::size_t clippedContourCapacity = contourCapacity * 2;

/// Clips the inset against each edge, removing corners whose edges disappear.
[[nodiscard]] std::size_t
clipConvexInset(
    std::span<const Vector2> points,
    std::span<const Vector2> normals,
    f32 inset,
    std::span<Vector2> output) noexcept;

/// Returns a counterclockwise convex boundary for the expanded contour.
[[nodiscard]] std::size_t
makeContourHull(std::span<Vector2> points, std::span<Vector2> hull) noexcept;

/// Returns the outward offset at a corner shared by two unit normals.
[[nodiscard]] Vector2
contourMiter(const Vector2& previous, const Vector2& next) noexcept;

/// Returns the quarter-circle segments for a bounded physical-pixel chord error.
[[nodiscard]] i32
quarterCircleSegments(f32 radius, f32 pixelRatio) noexcept;

/// Holds a range along one axis, such as a rectangle's left edge and width.
struct PixelSpan final {
    f32 start = 0.0f;
    f32 length = 0.0f;
};

/// Returns value, in logical pixels, moved to the nearest physical pixel edge.
[[nodiscard]] f32
snapToPixels(f32 value, f32 pixelRatio) noexcept;

/// Moves a span, in logical pixels, onto physical pixel edges. A span shorter
/// than a few physical pixels keeps its rounded length, at least one pixel,
/// so that a thin line neither disappears nor changes width as it moves. A
/// longer span rounds both ends, so that neighboring spans meet.
[[nodiscard]] PixelSpan
snapSpanToPixels(f32 start, f32 length, f32 pixelRatio) noexcept;

/// Returns thickness, in logical pixels, rounded to whole physical pixels, and
/// at least one.
[[nodiscard]] f32
snapThicknessToPixels(f32 thickness, f32 pixelRatio) noexcept;

/// Keeps corner samples paired when an inset radius or dimension collapses.
[[nodiscard]] Vector2
roundedContourPoint(f32 left, f32 bottom, f32 width, f32 height, f32 radius, i32 corner, f32 angle, f32 offset) noexcept;

/// Skips collapsed triangles left by a disappearing corner or core.
template <typename Triangle>
void emitAntialiasedTriangle(
    const Vector2& a, const Vector2& b, const Vector2& c,
    const Color& ca, const Color& cb, const Color& cc,
    Triangle& triangle)
{
    if (std::abs(math::cross(b - a, c - a)) > 0.000001f) {
        triangle(a, b, c, ca, cb, cc);
    }
}

/// Connects nested convex contours even when their vertex counts differ.
template <typename Triangle>
void drawAntialiasedContourRing(
    std::span<const Vector2> inner,
    std::span<const Vector2> outer,
    const Color& color,
    Triangle& triangle)
{
    auto transparent = color;
    transparent.a = 0;
    if (inner.size() == 1) {
        for (std::size_t i = 0; i < outer.size(); ++i) {
            emitAntialiasedTriangle(inner[0], outer[i], outer[(i + 1) % outer.size()], color, transparent, transparent, triangle);
        }
        return;
    }
    auto center = Vector2{0.0f, 0.0f};
    for (const auto& point : inner) {
        center += point;
    }
    center /= static_cast<f32>(inner.size());
    std::array<f32, clippedContourCapacity> innerAngles;
    std::array<f32, clippedContourCapacity> outerAngles;
    const auto angles = [&center](std::span<const Vector2> points, std::span<f32> result) {
        std::size_t start = 0;
        for (std::size_t i = 0; i < points.size(); ++i) {
            const auto offset = points[i] - center;
            auto angle = std::atan2(offset.y, offset.x);
            if (angle < 0.0f) {
                angle += math::TwoPi<f32>;
            }
            result[i] = angle;
            if (angle < result[start]) {
                start = i;
            }
        }
        return start;
    };
    const auto innerStart = angles(inner, innerAngles);
    const auto outerStart = angles(outer, outerAngles);
    std::size_t innerStep = 0;
    std::size_t outerStep = 0;
    while ((innerStep < inner.size()) || (outerStep < outer.size())) {
        const auto i = (innerStart + innerStep) % inner.size();
        const auto o = (outerStart + outerStep) % outer.size();
        const auto nextInner = (i + 1) % inner.size();
        const auto nextOuter = (o + 1) % outer.size();
        auto innerAngle = innerAngles[nextInner];
        auto outerAngle = outerAngles[nextOuter];
        if (nextInner == innerStart) {
            innerAngle += math::TwoPi<f32>;
        }
        if (nextOuter == outerStart) {
            outerAngle += math::TwoPi<f32>;
        }
        if ((innerStep < inner.size()) && ((outerStep == outer.size()) || (innerAngle < outerAngle))) {
            emitAntialiasedTriangle(inner[i], outer[o], inner[nextInner], color, transparent, color, triangle);
            ++innerStep;
        }
        else {
            emitAntialiasedTriangle(inner[i], outer[o], outer[nextOuter], color, transparent, transparent, triangle);
            ++outerStep;
        }
    }
}

/// Tessellates a convex silhouette with a centered alpha fringe.
template <typename Triangle>
void drawAntialiasedConvex(
    std::span<const Vector2> points,
    const Color& color,
    f32 fringeWidth,
    Triangle&& triangle)
{
    if ((points.size() < 3) || (points.size() > contourCapacity) || (color.a == 0)) {
        return;
    }
    if (fringeWidth <= 0.0f) {
        for (std::size_t i = 2; i < points.size(); ++i) {
            triangle(points[0], points[i - 1], points[i], color, color, color);
        }
        return;
    }

    std::array<Vector2, contourCapacity> normals;
    std::array<Vector2, contourCapacity> inner;
    std::array<Vector2, contourCapacity> outer;
    auto center = Vector2{0.0f, 0.0f};
    f32 area = 0.0f;
    for (std::size_t i = 0; i < points.size(); ++i) {
        const auto next = (i + 1) % points.size();
        center += points[i];
        area += math::cross(points[i] - points[0], points[next] - points[0]);
    }
    if (std::abs(area) <= 0.000001f) {
        return;
    }
    center /= static_cast<f32>(points.size());
    f32 orientation = 1.0f;
    if (area < 0.0f) {
        orientation = -1.0f;
    }

    auto halfFringe = fringeWidth * 0.5f;
    for (std::size_t i = 0; i < points.size(); ++i) {
        const auto edge = points[(i + 1) % points.size()] - points[i];
        const auto length = math::length(edge);
        if (length <= 0.000001f) {
            return;
        }
        normals[i] = Vector2{edge.y, -edge.x} * (orientation / length);
        // NOTE: Keep the inset inside small convex silhouettes.
        halfFringe = std::min(halfFringe, std::abs(math::dot(center - points[i], normals[i])));
    }
    auto solid = color;
    if (halfFringe < fringeWidth * 0.5f) {
        solid.a = static_cast<u8>(std::round(static_cast<f32>(color.a) * (2.0f * halfFringe / fringeWidth)));
    }
    for (std::size_t i = 0; i < points.size(); ++i) {
        const auto previous = (i + points.size() - 1) % points.size();
        const auto miter = contourMiter(normals[previous], normals[i]);
        inner[i] = points[i] - miter * halfFringe;
        outer[i] = points[i] + miter * (fringeWidth - halfFringe);
    }
    auto hasArea = false;
    auto folded = false;
    for (std::size_t i = 0; i < points.size(); ++i) {
        const auto next = (i + 1) % points.size();
        const auto following = (i + 2) % points.size();
        const auto edge = inner[next] - inner[i];
        const auto turn = math::cross(edge, inner[following] - inner[next]) * orientation;
        hasArea = hasArea || (turn > 0.000001f);
        folded = folded || (turn < -0.000001f) || (math::dot(edge, points[next] - points[i]) < -0.000001f);
    }
    if (folded || !hasArea) {
        // NOTE: A feasible inset can still lose a short edge. Clip it before
        // triangulation instead of treating the reversed edge as a convex fan.
        std::array<Vector2, clippedContourCapacity> clipped;
        auto count = clipConvexInset(points, std::span<const Vector2>{normals}.first(points.size()), halfFringe, clipped);
        if (count == 0) {
            clipped[0] = center;
            count = 1;
        }
        const auto insetContour = std::span<Vector2>{clipped}.first(count);
        if (orientation < 0.0f) {
            std::reverse(insetContour.begin(), insetContour.end());
        }
        for (std::size_t i = 2; i < count; ++i) {
            emitAntialiasedTriangle(clipped[0], clipped[i - 1], clipped[i], solid, solid, solid, triangle);
        }
        std::array<Vector2, clippedContourCapacity> hull;
        const auto outerCount = makeContourHull(std::span<Vector2>{outer}.first(points.size()), hull);
        drawAntialiasedContourRing(std::span<const Vector2>{clipped}.first(count), std::span<const Vector2>{hull}.first(outerCount), solid, triangle);
        return;
    }
    for (std::size_t i = 2; i < points.size(); ++i) {
        emitAntialiasedTriangle(inner[0], inner[i - 1], inner[i], solid, solid, solid, triangle);
    }
    auto transparent = solid;
    transparent.a = 0;
    for (std::size_t i = 0; i < points.size(); ++i) {
        const auto next = (i + 1) % points.size();
        emitAntialiasedTriangle(inner[i], outer[i], outer[next], solid, transparent, transparent, triangle);
        emitAntialiasedTriangle(inner[i], outer[next], inner[next], solid, transparent, solid, triangle);
    }
}

/// Tessellates rounded fills with clamped contours instead of a polygon inset.
template <typename Triangle>
void drawAntialiasedRoundedRect(
    f32 left, f32 bottom, f32 width, f32 height, f32 radius,
    const Color& color, f32 fringeWidth, f32 pixelRatio,
    Triangle&& triangle)
{
    if ((width <= 0.0f) || (height <= 0.0f) || (color.a == 0)) {
        return;
    }
    const auto r = std::clamp(radius, 0.0f, std::min(width, height) * 0.5f);
    const auto segments = quarterCircleSegments(r, pixelRatio);
    const auto aa = std::max(fringeWidth, 0.0f);
    const auto inset = std::min(aa * 0.5f, std::min(width, height) * 0.5f);
    auto solid = color;
    if ((aa > 0.0f) && (inset < aa * 0.5f)) {
        solid.a = static_cast<u8>(std::round(static_cast<f32>(color.a) * (2.0f * inset / aa)));
    }
    auto transparent = solid;
    transparent.a = 0;
    std::array<Vector2, contourCapacity> inner;
    std::array<Vector2, contourCapacity> outer;
    std::size_t count = 0;
    for (i32 corner = 0; corner < 4; ++corner) {
        for (i32 i = 0; i <= segments; ++i) {
            const auto angle = math::PiOver2<f32> * (static_cast<f32>(corner) + static_cast<f32>(i) / static_cast<f32>(segments));
            inner[count] = roundedContourPoint(left, bottom, width, height, r, corner, angle, -inset);
            outer[count] = roundedContourPoint(left, bottom, width, height, r, corner, angle, aa - inset);
            ++count;
        }
    }
    for (std::size_t i = 2; i < count; ++i) {
        emitAntialiasedTriangle(inner[0], inner[i - 1], inner[i], solid, solid, solid, triangle);
    }
    if (aa <= 0.0f) {
        return;
    }
    for (std::size_t i = 0; i < count; ++i) {
        const auto next = (i + 1) % count;
        emitAntialiasedTriangle(inner[i], outer[i], outer[next], solid, transparent, transparent, triangle);
        emitAntialiasedTriangle(inner[i], outer[next], inner[next], solid, transparent, solid, triangle);
    }
}

/// Tessellates two or three stroke points with a shared miter and butt caps.
template <typename Triangle>
void drawAntialiasedPolyline(
    std::span<const Vector2> points,
    const Color& color,
    f32 thickness,
    f32 fringeWidth,
    Triangle&& triangle)
{
    if ((points.size() < 2) || (points.size() > 3) || (thickness <= 0.0f) || (color.a == 0)) {
        return;
    }
    std::array<Vector2, 3> path{Vector2{0.0f, 0.0f}, Vector2{0.0f, 0.0f}, Vector2{0.0f, 0.0f}};
    std::size_t count = 0;
    for (const auto& point : points) {
        if ((count == 0) || (math::distanceSquared(path[count - 1], point) > 0.000001f)) {
            path[count++] = point;
        }
    }
    if (count < 2) {
        return;
    }

    std::array<Vector2, 2> directions;
    std::array<Vector2, 3> offsets;
    for (std::size_t i = 0; i + 1 < count; ++i) {
        directions[i] = math::normalize(path[i + 1] - path[i]);
    }
    offsets[0] = Vector2{-directions[0].y, directions[0].x};
    offsets[count - 1] = Vector2{-directions[count - 2].y, directions[count - 2].x};
    if (count == 3) {
        if (math::dot(directions[0], directions[1]) < -0.99f) {
            // NOTE: A reversing path covers the same stroke twice. Collapse it
            // to its longer segment rather than producing an unbounded miter.
            std::array<Vector2, 2> segment{path[1], path[0]};
            if (math::distanceSquared(path[1], path[2]) > math::distanceSquared(path[1], path[0])) {
                segment[1] = path[2];
            }
            drawAntialiasedPolyline(std::span<const Vector2>{segment}, color, thickness, fringeWidth, triangle);
            return;
        }
        offsets[1] = contourMiter(offsets[0], offsets[2]);
    }

    const auto aa = std::max(0.0f, fringeWidth);
    const auto innerHalf = std::max(0.0f, (thickness - aa) * 0.5f);
    const auto outerHalf = innerHalf + aa;
    auto solid = color;
    if ((aa > 0.0f) && (thickness < aa)) {
        solid.a = static_cast<u8>(std::round(static_cast<f32>(color.a) * thickness / aa));
    }
    auto transparent = solid;
    transparent.a = 0;
    std::array<Vector2, 3> centers = path;
    const auto startCap = std::min(aa * 0.5f, math::distance(path[0], path[1]) * 0.5f);
    const auto endCap = std::min(aa * 0.5f, math::distance(path[count - 2], path[count - 1]) * 0.5f);
    if (count == 3) {
        // NOTE: A thick, short bend can fold a strip over itself. Use a bounded
        // convex envelope for this case so translucent triangles do not overlap.
        const auto extension = outerHalf * std::max(std::abs(math::dot(offsets[1], directions[0])), std::abs(math::dot(offsets[1], directions[1])));
        const auto available = std::min(math::distance(path[0], path[1]) - startCap, math::distance(path[1], path[2]) - endCap);
        if (extension > available) {
            const auto halfWidth = thickness * 0.5f;
            std::array<Vector2, 6> corners{
                path[0] + offsets[0] * halfWidth, path[0] - offsets[0] * halfWidth,
                path[1] + offsets[1] * halfWidth, path[1] - offsets[1] * halfWidth,
                path[2] + offsets[2] * halfWidth, path[2] - offsets[2] * halfWidth};
            std::sort(corners.begin(), corners.end(), [](const Vector2& a, const Vector2& b) {
                return (a.x < b.x) || ((a.x == b.x) && (a.y < b.y));
            });
            std::array<Vector2, 12> hull;
            std::size_t hullCount = 0;
            for (const auto& corner : corners) {
                while ((hullCount >= 2) && (math::cross(hull[hullCount - 1] - hull[hullCount - 2], corner - hull[hullCount - 1]) <= 0.0f)) {
                    --hullCount;
                }
                hull[hullCount++] = corner;
            }
            const auto upperBegin = hullCount + 1;
            for (auto i = corners.size() - 1; i > 0; --i) {
                const auto& corner = corners[i - 1];
                while ((hullCount >= upperBegin) && (math::cross(hull[hullCount - 1] - hull[hullCount - 2], corner - hull[hullCount - 1]) <= 0.0f)) {
                    --hullCount;
                }
                hull[hullCount++] = corner;
            }
            drawAntialiasedConvex(std::span<const Vector2>{hull}.first(hullCount - 1), color, aa, triangle);
            return;
        }
    }
    centers[0] += directions[0] * startCap;
    centers[count - 1] -= directions[count - 2] * endCap;

    for (std::size_t i = 0; i + 1 < count; ++i) {
        const auto j = i + 1;
        const auto left0 = centers[i] + offsets[i] * innerHalf;
        const auto right0 = centers[i] - offsets[i] * innerHalf;
        const auto left1 = centers[j] + offsets[j] * innerHalf;
        const auto right1 = centers[j] - offsets[j] * innerHalf;
        triangle(left0, right0, right1, solid, solid, solid);
        triangle(left0, right1, left1, solid, solid, solid);
        if (aa <= 0.0f) {
            continue;
        }
        const auto outerLeft0 = centers[i] + offsets[i] * outerHalf;
        const auto outerRight0 = centers[i] - offsets[i] * outerHalf;
        const auto outerLeft1 = centers[j] + offsets[j] * outerHalf;
        const auto outerRight1 = centers[j] - offsets[j] * outerHalf;
        triangle(left0, left1, outerLeft1, solid, solid, transparent);
        triangle(left0, outerLeft1, outerLeft0, solid, transparent, transparent);
        triangle(right0, outerRight0, outerRight1, solid, transparent, transparent);
        triangle(right0, outerRight1, right1, solid, transparent, solid);
    }
    if (aa <= 0.0f) {
        return;
    }
    for (const auto endpoint : {std::size_t{0}, count - 1}) {
        auto outside = path[endpoint];
        if (endpoint == 0) {
            outside -= directions[0] * startCap;
        }
        else {
            outside += directions[count - 2] * endCap;
        }
        const auto left = centers[endpoint] + offsets[endpoint] * innerHalf;
        const auto right = centers[endpoint] - offsets[endpoint] * innerHalf;
        const auto outerLeft = centers[endpoint] + offsets[endpoint] * outerHalf;
        const auto outerRight = centers[endpoint] - offsets[endpoint] * outerHalf;
        const auto capLeft = outside + offsets[endpoint] * innerHalf;
        const auto capRight = outside - offsets[endpoint] * innerHalf;
        const auto capOuterLeft = outside + offsets[endpoint] * outerHalf;
        const auto capOuterRight = outside - offsets[endpoint] * outerHalf;
        triangle(left, capLeft, capRight, solid, transparent, transparent);
        triangle(left, capRight, right, solid, transparent, solid);
        triangle(left, outerLeft, capOuterLeft, solid, transparent, transparent);
        triangle(left, capOuterLeft, capLeft, solid, transparent, transparent);
        triangle(right, capRight, capOuterRight, solid, transparent, transparent);
        triangle(right, capOuterRight, outerRight, solid, transparent, transparent);
    }
}

} // namespace pomdog::devgui::detail
