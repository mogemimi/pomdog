// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/experimental/devgui/antialiasing.h"
#include "pomdog/math/color.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>
#include <span>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Color;
using pomdog::f32;
using pomdog::u32;
using pomdog::Vector2;
using pomdog::devgui::detail::drawAntialiasedConvex;
using pomdog::devgui::detail::drawAntialiasedPolyline;
using pomdog::devgui::detail::drawAntialiasedRoundedRect;

namespace {

struct Triangle final {
    std::array<Vector2, 3> points;
    std::array<Color, 3> colors;
};

struct Coverage final {
    f32 alpha = 0.0f;
    u32 count = 0;
};

[[nodiscard]] f32
edge(const Vector2& start, const Vector2& end, const Vector2& point) noexcept
{
    return (end.x - start.x) * (point.y - start.y) - (end.y - start.y) * (point.x - start.x);
}

[[nodiscard]] bool
containsEdge(const Vector2& start, const Vector2& end, f32 distance) noexcept
{
    if (std::abs(distance) > 0.00001f) {
        return distance > 0.0f;
    }
    // NOTE: Assign a shared edge to exactly one triangle, as a rasterizer does.
    const auto delta = end - start;
    return (delta.y > 0.0f) || ((delta.y == 0.0f) && (delta.x < 0.0f));
}

[[nodiscard]] Coverage
sampleCoverage(const std::vector<Triangle>& triangles, const Vector2& point) noexcept
{
    Coverage coverage;
    for (const auto& triangle : triangles) {
        auto indexes = std::array<std::size_t, 3>{{0, 1, 2}};
        auto area = edge(triangle.points[0], triangle.points[1], triangle.points[2]);
        if (std::abs(area) <= 0.00001f) {
            continue;
        }
        if (area < 0.0f) {
            indexes = {{0, 2, 1}};
            area = -area;
        }
        const auto& p0 = triangle.points[indexes[0]];
        const auto& p1 = triangle.points[indexes[1]];
        const auto& p2 = triangle.points[indexes[2]];
        const auto e0 = edge(p0, p1, point);
        const auto e1 = edge(p1, p2, point);
        const auto e2 = edge(p2, p0, point);
        if (!containsEdge(p0, p1, e0) || !containsEdge(p1, p2, e1) || !containsEdge(p2, p0, e2)) {
            continue;
        }
        const auto alpha =
            (e1 * static_cast<f32>(triangle.colors[indexes[0]].a) +
                e2 * static_cast<f32>(triangle.colors[indexes[1]].a) +
                e0 * static_cast<f32>(triangle.colors[indexes[2]].a)) /
            (area * 255.0f);
        if (alpha > 0.00001f) {
            coverage.alpha += alpha;
            ++coverage.count;
        }
    }
    return coverage;
}

[[nodiscard]] std::vector<Triangle>
fillConvex(std::span<const Vector2> points, const Color& color, f32 fringeWidth)
{
    std::vector<Triangle> triangles;
    drawAntialiasedConvex(points, color, fringeWidth,
        [&](const Vector2& p0, const Vector2& p1, const Vector2& p2, const Color& c0, const Color& c1, const Color& c2) {
            triangles.push_back(Triangle{{p0, p1, p2}, {c0, c1, c2}});
        });
    return triangles;
}

[[nodiscard]] std::vector<Triangle>
strokePolyline(std::span<const Vector2> points, const Color& color, f32 thickness, f32 fringeWidth)
{
    std::vector<Triangle> triangles;
    drawAntialiasedPolyline(points, color, thickness, fringeWidth,
        [&](const Vector2& p0, const Vector2& p1, const Vector2& p2, const Color& c0, const Color& c1, const Color& c2) {
            triangles.push_back(Triangle{{p0, p1, p2}, {c0, c1, c2}});
        });
    return triangles;
}

[[nodiscard]] std::vector<Triangle>
fillRoundedRect(f32 width, f32 height, f32 radius, const Color& color, f32 fringeWidth, f32 pixelRatio)
{
    std::vector<Triangle> triangles;
    drawAntialiasedRoundedRect(0.0f, 0.0f, width, height, radius, color, fringeWidth, pixelRatio,
        [&](const Vector2& p0, const Vector2& p1, const Vector2& p2, const Color& c0, const Color& c1, const Color& c2) {
            triangles.push_back(Triangle{{p0, p1, p2}, {c0, c1, c2}});
        });
    return triangles;
}

void checkFinite(const std::vector<Triangle>& triangles)
{
    for (const auto& triangle : triangles) {
        for (const auto& point : triangle.points) {
            REQUIRE(std::isfinite(point.x));
            REQUIRE(std::isfinite(point.y));
        }
    }
}

void checkSingleCoverage(const std::vector<Triangle>& triangles, const Vector2& minimum, const Vector2& size, f32 opacity)
{
    for (pomdog::i32 y = 0; y < 80; ++y) {
        for (pomdog::i32 x = 0; x < 80; ++x) {
            const auto point = Vector2{
                minimum.x + (static_cast<f32>(x) + 0.37f) * size.x / 80.0f,
                minimum.y + (static_cast<f32>(y) + 0.63f) * size.y / 80.0f};
            const auto coverage = sampleCoverage(triangles, point);
            REQUIRE((coverage.count <= 1));
            REQUIRE((coverage.alpha <= opacity + 0.0001f));
        }
    }
}

} // namespace

TEST_CASE("devgui::detail::drawAntialiasedConvex")
{
    const auto points = std::array<Vector2, 4>{Vector2{0.0f, 0.0f}, Vector2{8.0f, 0.0f}, Vector2{8.0f, 8.0f}, Vector2{0.0f, 8.0f}};
    const auto color = Color{37, 127, 213, 173};
    const auto opacity = static_cast<f32>(color.a) / 255.0f;

    SUBCASE("only the contour changes coverage, including across fan seams")
    {
        const auto triangles = fillConvex(points, color, 1.0f);
        REQUIRE_FALSE(triangles.empty());
        for (const auto& point : std::array<Vector2, 4>{Vector2{4.0f, 4.0f}, Vector2{2.0f, 2.0f}, Vector2{6.0f, 6.0f}, Vector2{2.25f, 5.75f}}) {
            const auto coverage = sampleCoverage(triangles, point);
            REQUIRE(coverage.count == 1);
            REQUIRE(coverage.alpha == doctest::Approx(opacity));
        }
        REQUIRE(sampleCoverage(triangles, Vector2{4.0f, -0.75f}).count == 0);
        REQUIRE(sampleCoverage(triangles, Vector2{4.0f, 0.0f}).alpha == doctest::Approx(opacity * 0.5f));
    }
    SUBCASE("smoothing stays one physical pixel wide across DPI ratios")
    {
        for (const auto pixelRatio : std::array<f32, 3>{{1.0f, 1.25f, 2.0f}}) {
            const auto triangles = fillConvex(points, color, 1.0f / pixelRatio);
            REQUIRE(sampleCoverage(triangles, Vector2{4.0f, -0.25f / pixelRatio}).alpha == doctest::Approx(opacity * 0.25f));
            REQUIRE(sampleCoverage(triangles, Vector2{4.0f, 0.25f / pixelRatio}).alpha == doctest::Approx(opacity * 0.75f));
            REQUIRE(sampleCoverage(triangles, Vector2{4.0f, -0.75f / pixelRatio}).count == 0);
        }
    }
    SUBCASE("transparent fringe keeps the straight-alpha RGB")
    {
        const auto triangles = fillConvex(points, color, 1.0f);
        auto foundTransparent = false;
        for (const auto& triangle : triangles) {
            for (const auto& vertexColor : triangle.colors) {
                REQUIRE(vertexColor.r == color.r);
                REQUIRE(vertexColor.g == color.g);
                REQUIRE(vertexColor.b == color.b);
                foundTransparent = foundTransparent || (vertexColor.a == 0);
            }
        }
        REQUIRE(foundTransparent);
    }
    SUBCASE("opposite winding has the same coverage")
    {
        const auto reverse = std::array<Vector2, 4>{points[3], points[2], points[1], points[0]};
        const auto triangles = fillConvex(reverse, color, 1.0f);
        REQUIRE(sampleCoverage(triangles, Vector2{4.0f, 4.0f}).alpha == doctest::Approx(opacity));
        REQUIRE(sampleCoverage(triangles, Vector2{4.0f, 0.0f}).alpha == doctest::Approx(opacity * 0.5f));
        checkFinite(triangles);
    }
    SUBCASE("degenerate contours do not emit nonfinite geometry")
    {
        const auto line = std::array<Vector2, 3>{Vector2{0.0f, 0.0f}, Vector2{4.0f, 0.0f}, Vector2{8.0f, 0.0f}};
        checkFinite(fillConvex(line, color, 1.0f));
        const auto point = std::array<Vector2, 3>{Vector2{4.0f, 4.0f}, Vector2{4.0f, 4.0f}, Vector2{4.0f, 4.0f}};
        checkFinite(fillConvex(point, color, 1.0f));
        const auto emptyTriangles = fillConvex(std::span<const Vector2>{}, color, 1.0f);
        REQUIRE(emptyTriangles.empty());
    }
    SUBCASE("subpixel silhouettes fade while retaining the smoothing band")
    {
        const auto small = std::array<Vector2, 4>{Vector2{0.0f, 0.0f}, Vector2{0.25f, 0.0f}, Vector2{0.25f, 0.25f}, Vector2{0.0f, 0.25f}};
        const auto triangles = fillConvex(small, color, 1.0f);
        const auto center = sampleCoverage(triangles, Vector2{0.125f, 0.125f});
        REQUIRE(center.count == 1);
        REQUIRE(center.alpha > 0.0f);
        REQUIRE(center.alpha < opacity * 0.5f);
        REQUIRE(sampleCoverage(triangles, Vector2{0.125f, -0.5f}).alpha > 0.0f);
        checkFinite(triangles);
    }
    SUBCASE("a disappearing bevel edge leaves a convex core and one fringe")
    {
        const auto bevel = std::array<Vector2, 5>{Vector2{0.0f, 0.0f}, Vector2{10.0f, 0.0f}, Vector2{10.0f, 10.0f}, Vector2{0.1f, 10.0f}, Vector2{0.0f, 9.9f}};
        const auto triangles = fillConvex(bevel, color, 1.0f);
        REQUIRE(sampleCoverage(triangles, Vector2{5.0f, 5.0f}).alpha == doctest::Approx(opacity));
        const auto formerOverlap = sampleCoverage(triangles, Vector2{0.318425f, 9.001575f});
        REQUIRE(formerOverlap.count == 1);
        REQUIRE((formerOverlap.alpha <= opacity));
        checkSingleCoverage(triangles, Vector2{-0.6f, -0.6f}, Vector2{11.2f, 11.2f}, opacity);
        for (std::size_t start = 0; start < bevel.size(); ++start) {
            auto rotated = bevel;
            const auto rotatedSpan = std::span<Vector2>{rotated};
            std::rotate(rotatedSpan.begin(), std::next(rotatedSpan.begin(), static_cast<std::ptrdiff_t>(start)), rotatedSpan.end());
            const auto counterclockwise = fillConvex(rotated, color, 1.0f);
            checkSingleCoverage(counterclockwise, Vector2{-0.6f, -0.6f}, Vector2{11.2f, 11.2f}, opacity);
            std::reverse(rotatedSpan.begin(), rotatedSpan.end());
            const auto clockwise = fillConvex(rotated, color, 1.0f);
            checkSingleCoverage(clockwise, Vector2{-0.6f, -0.6f}, Vector2{11.2f, 11.2f}, opacity);
        }
        checkFinite(triangles);
    }
    SUBCASE("a narrow convex capsule can collapse its core to a line")
    {
        const auto capsule = std::array<Vector2, 6>{Vector2{0.5f, 0.0f}, Vector2{1.0f, 0.5f}, Vector2{1.0f, 9.5f}, Vector2{0.5f, 10.0f}, Vector2{0.0f, 9.5f}, Vector2{0.0f, 0.5f}};
        const auto triangles = fillConvex(capsule, color, 1.0f);
        REQUIRE_FALSE(triangles.empty());
        checkSingleCoverage(triangles, Vector2{-0.6f, -0.6f}, Vector2{2.2f, 11.2f}, opacity);
        checkFinite(triangles);
    }
}

TEST_CASE("devgui::detail::drawAntialiasedRoundedRect")
{
    const auto color = Color{37, 127, 213, 128};
    const auto opacity = static_cast<f32>(color.a) / 255.0f;

    SUBCASE("half-pixel corners collapse without overlapping translucent triangles")
    {
        const auto triangles = fillRoundedRect(10.0f, 10.0f, 0.5f, color, 1.0f, 1.0f);
        const auto formerOverlap = sampleCoverage(triangles, Vector2{1.68185f, 9.47815f});
        REQUIRE(formerOverlap.count == 1);
        REQUIRE((formerOverlap.alpha <= opacity));
        checkSingleCoverage(triangles, Vector2{-0.6f, -0.6f}, Vector2{11.2f, 11.2f}, opacity);
        REQUIRE(sampleCoverage(triangles, Vector2{5.0f, 0.0f}).alpha == doctest::Approx(opacity * 0.5f));
        REQUIRE(sampleCoverage(triangles, Vector2{5.0f, -0.6f}).count == 0);
        checkFinite(triangles);
    }
    SUBCASE("a one-pixel capsule retains one fragment at its collapsed ends")
    {
        const auto triangles = fillRoundedRect(1.0f, 10.0f, 0.5f, color, 1.0f, 1.0f);
        const auto formerOverlap = sampleCoverage(triangles, Vector2{0.49244f, 9.49556f});
        REQUIRE(formerOverlap.count == 1);
        REQUIRE((formerOverlap.alpha <= opacity));
        checkSingleCoverage(triangles, Vector2{-0.6f, -0.6f}, Vector2{2.2f, 11.2f}, opacity);
        REQUIRE(sampleCoverage(triangles, Vector2{0.5f, 5.0f}).alpha == doctest::Approx(opacity));
        checkFinite(triangles);
    }
    SUBCASE("the transition remains centered and one physical pixel wide")
    {
        for (const auto pixelRatio : std::array<f32, 3>{{1.0f, 1.25f, 2.0f}}) {
            const auto triangles = fillRoundedRect(10.0f, 10.0f, 0.5f, color, 1.0f / pixelRatio, pixelRatio);
            REQUIRE(sampleCoverage(triangles, Vector2{5.0f, 0.0f}).alpha == doctest::Approx(opacity * 0.5f));
            REQUIRE(sampleCoverage(triangles, Vector2{5.0f, 0.25f / pixelRatio}).alpha == doctest::Approx(opacity * 0.75f));
            REQUIRE(sampleCoverage(triangles, Vector2{5.0f, -0.25f / pixelRatio}).alpha == doctest::Approx(opacity * 0.25f));
            checkSingleCoverage(triangles, Vector2{-0.6f, -0.6f}, Vector2{11.2f, 11.2f}, opacity);
        }
    }
    SUBCASE("subpixel widths fade without changing the straight-alpha RGB")
    {
        const auto triangles = fillRoundedRect(0.5f, 10.0f, 0.25f, color, 1.0f, 1.0f);
        const auto center = sampleCoverage(triangles, Vector2{0.25f, 5.0f});
        REQUIRE(center.count == 1);
        REQUIRE(center.alpha == doctest::Approx(opacity * 0.5f));
        checkSingleCoverage(triangles, Vector2{-0.8f, -0.8f}, Vector2{2.1f, 11.6f}, opacity * 0.5f);
        for (const auto& triangle : triangles) {
            for (const auto& vertexColor : triangle.colors) {
                REQUIRE(vertexColor.r == color.r);
                REQUIRE(vertexColor.g == color.g);
                REQUIRE(vertexColor.b == color.b);
            }
        }
    }
}

TEST_CASE("devgui::detail::drawAntialiasedPolyline")
{
    const auto color = Color{37, 127, 213, 153};
    const auto opacity = static_cast<f32>(color.a) / 255.0f;
    const auto line = std::array<Vector2, 2>{Vector2{0.0f, 0.0f}, Vector2{16.0f, 0.0f}};

    SUBCASE("the side transition is centered on the requested stroke width")
    {
        const auto triangles = strokePolyline(line, color, 4.0f, 1.0f);
        REQUIRE(sampleCoverage(triangles, Vector2{4.25f, 0.0f}).alpha == doctest::Approx(opacity));
        REQUIRE(sampleCoverage(triangles, Vector2{4.25f, 2.0f}).alpha == doctest::Approx(opacity * 0.5f));
        REQUIRE(sampleCoverage(triangles, Vector2{4.25f, -2.0f}).alpha == doctest::Approx(opacity * 0.5f));
        REQUIRE(sampleCoverage(triangles, Vector2{4.25f, 2.75f}).count == 0);
    }
    SUBCASE("subpixel strokes fade instead of becoming fully opaque")
    {
        const auto triangles = strokePolyline(line, color, 0.25f, 1.0f);
        REQUIRE_FALSE(triangles.empty());
        const auto coverage = sampleCoverage(triangles, Vector2{4.25f, 0.0f});
        REQUIRE(coverage.count == 1);
        REQUIRE(coverage.alpha > 0.0f);
        REQUIRE(coverage.alpha < opacity * 0.5f);
        REQUIRE(sampleCoverage(triangles, Vector2{4.25f, 2.0f}).count == 0);
        checkFinite(triangles);
    }
    SUBCASE("a joined check mark has no overlapping translucent segments")
    {
        const auto points = std::array<Vector2, 3>{Vector2{0.0f, 0.0f}, Vector2{8.0f, 8.0f}, Vector2{16.0f, 0.0f}};
        const auto triangles = strokePolyline(points, color, 4.0f, 1.0f);
        REQUIRE_FALSE(triangles.empty());
        REQUIRE(sampleCoverage(triangles, Vector2{8.0f, 7.9f}).alpha == doctest::Approx(opacity));
        for (pomdog::i32 y = 0; y < 40; ++y) {
            for (pomdog::i32 x = 0; x < 40; ++x) {
                const auto point = Vector2{6.05f + static_cast<f32>(x) * 0.1f, 6.05f + static_cast<f32>(y) * 0.1f};
                const auto coverage = sampleCoverage(triangles, point);
                REQUIRE((coverage.count <= 1));
                REQUIRE((coverage.alpha <= opacity + 0.0001f));
            }
        }
    }
    SUBCASE("zero-length segments and reversals keep all coordinates finite")
    {
        const auto duplicate = std::array<Vector2, 3>{line[0], line[0], line[1]};
        const auto triangles = strokePolyline(duplicate, color, 4.0f, 1.0f);
        checkFinite(triangles);
        REQUIRE(sampleCoverage(triangles, Vector2{4.25f, 0.0f}).alpha == doctest::Approx(opacity));
        const auto reversal = std::array<Vector2, 3>{line[0], line[1], line[0]};
        checkFinite(strokePolyline(reversal, color, 4.0f, 1.0f));
        const auto almostReversal = std::array<Vector2, 3>{line[0], line[1], Vector2{0.0f, 0.0001f}};
        checkFinite(strokePolyline(almostReversal, color, 4.0f, 1.0f));
        const auto point = std::array<Vector2, 2>{line[0], line[0]};
        const auto pointTriangles = strokePolyline(point, color, 4.0f, 1.0f);
        const auto zeroThickness = strokePolyline(line, color, 0.0f, 1.0f);
        const auto emptyTriangles = strokePolyline(std::span<const Vector2>{}, color, 4.0f, 1.0f);
        REQUIRE(pointTriangles.empty());
        REQUIRE(zeroThickness.empty());
        REQUIRE(emptyTriangles.empty());
    }
    SUBCASE("short segments keep the joined stroke from covering a pixel twice")
    {
        const auto points = std::array<Vector2, 3>{Vector2{0.0f, 0.0f}, Vector2{1.0f, 0.0f}, Vector2{1.0f, 1.0f}};
        const auto triangles = strokePolyline(points, color, 4.0f, 1.0f);
        REQUIRE_FALSE(triangles.empty());
        checkFinite(triangles);
        for (pomdog::i32 y = 0; y < 60; ++y) {
            for (pomdog::i32 x = 0; x < 60; ++x) {
                const auto point = Vector2{-1.95f + static_cast<f32>(x) * 0.1f, -1.95f + static_cast<f32>(y) * 0.1f};
                const auto coverage = sampleCoverage(triangles, point);
                REQUIRE((coverage.count <= 1));
                REQUIRE((coverage.alpha <= opacity + 0.0001f));
            }
        }
    }
}

TEST_CASE("devgui::detail::snapSpanToPixels")
{
    using pomdog::devgui::detail::snapSpanToPixels;
    using pomdog::devgui::detail::snapThicknessToPixels;
    using pomdog::devgui::detail::snapToPixels;

    SUBCASE("keeps a thin span one width wherever it lies")
    {
        // NOTE: At a pixel ratio of 1.5, a one-pixel border covers two physical
        // pixels at x = 10 and at x = 10.667, where rounding both ends would
        // give one of them a single pixel.
        const auto a = snapSpanToPixels(10.0f, 1.0f, 1.5f);
        const auto b = snapSpanToPixels(32.0f / 3.0f, 1.0f, 1.5f);
        REQUIRE(a.start * 1.5f == doctest::Approx(15.0f));
        REQUIRE(a.length * 1.5f == doctest::Approx(2.0f));
        REQUIRE(b.start * 1.5f == doctest::Approx(16.0f));
        REQUIRE(b.length * 1.5f == doctest::Approx(2.0f));
    }
    SUBCASE("keeps at least one physical pixel")
    {
        const auto span = snapSpanToPixels(0.2f, 0.3f, 1.0f);
        REQUIRE(span.start == 0.0f);
        REQUIRE(span.length == 1.0f);
        REQUIRE(snapSpanToPixels(5.0f, 0.0f, 1.0f).length == 0.0f);
    }
    SUBCASE("rounds both ends of a wide span so that neighbors meet")
    {
        const auto left = snapSpanToPixels(0.4f, 10.2f, 1.0f);
        const auto right = snapSpanToPixels(10.6f, 10.0f, 1.0f);
        REQUIRE(left.start == 0.0f);
        REQUIRE(left.length == 11.0f);
        REQUIRE(right.start == 11.0f);
    }
    SUBCASE("rounds thickness to whole physical pixels")
    {
        REQUIRE(snapThicknessToPixels(1.0f, 1.5f) * 1.5f == doctest::Approx(2.0f));
        REQUIRE(snapThicknessToPixels(0.2f, 1.0f) == 1.0f);
        REQUIRE(snapToPixels(10.3f, 2.0f) == 10.5f);
    }
}
