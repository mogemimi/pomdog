// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/curve_editor.h"
#include "pomdog/experimental/devgui/style.h"
#include "pomdog/experimental/devgui/ui.h"
#include "pomdog/experimental/devgui/widget_helpers.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
#include <iterator>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
namespace {

/// The space between the graph frame and the plot, so that points on the edge
/// stay fully visible and easy to grab.
constexpr f32 plotInset = 8.0f;

/// The distance within which the pointer grabs a point.
constexpr f32 grabRadius = 8.0f;

constexpr f32 pointRadius = 4.5f;
constexpr f32 pointRadiusHot = 6.0f;

/// Maps between curve values and positions in the plot rectangle.
class CurveMapping final {
public:
    CurveMapping(const F32Rect2D& plot, f32 minY, f32 maxY) noexcept
        : plot_(plot)
        , minY_(minY)
        , rangeY_(std::max(maxY - minY, 1e-6f))
    {
    }

    [[nodiscard]] Vector2
    toScreen(f32 x, f32 y) const noexcept
    {
        const auto t = (y - minY_) / rangeY_;
        return Vector2{plot_.x + x * plot_.width, plot_.y + (1.0f - t) * plot_.height};
    }

    [[nodiscard]] CurvePoint
    toCurve(const Vector2& position) const noexcept
    {
        CurvePoint point;
        point.x = std::clamp((position.x - plot_.x) / std::max(plot_.width, 1.0f), 0.0f, 1.0f);
        const auto t = std::clamp(1.0f - (position.y - plot_.y) / std::max(plot_.height, 1.0f), 0.0f, 1.0f);
        point.y = minY_ + t * rangeY_;
        return point;
    }

private:
    F32Rect2D plot_;
    f32 minY_;
    f32 rangeY_;
};

/// Returns the index of the point within grabRadius of position, or -1.
[[nodiscard]] i32
findPointAt(const std::vector<CurvePoint>& points, const CurveMapping& mapping, const Vector2& position) noexcept
{
    i32 nearest = -1;
    f32 nearestDistance = grabRadius * grabRadius;
    for (std::size_t i = 0; i < points.size(); ++i) {
        const auto p = mapping.toScreen(points[i].x, points[i].y);
        const auto dx = p.x - position.x;
        const auto dy = p.y - position.y;
        const auto distance = dx * dx + dy * dy;
        if (distance <= nearestDistance) {
            nearest = static_cast<i32>(i);
            nearestDistance = distance;
        }
    }
    return nearest;
}

} // namespace

Response curveEditor(UI& ui, Key key, std::string_view label, std::vector<CurvePoint>& points, const CurveEditorOptions& options)
{
    const auto& style = ui.getStyle();
    const auto& pointer = ui.getInput().pointerPosition;
    const auto id = ui.makeID(key);
    const auto row = detail::layoutPropertyRow(ui, label, options.height);
    const auto frame = row.control;
    auto response = ui.interact(id, frame);

    const auto plot = F32Rect2D{
        frame.x + plotInset,
        frame.y + plotInset,
        std::max(0.0f, frame.width - plotInset * 2.0f),
        std::max(0.0f, frame.height - plotInset * 2.0f),
    };
    const auto mapping = CurveMapping{plot, options.minY, options.maxY};

    // NOTE: The active scratch value holds the index of the dragged point plus
    // one, or zero while no point is dragged.
    auto& dragged = ui.getActiveScratch();
    if (response.activated) {
        const auto index = findPointAt(points, mapping, pointer);
        dragged = 0.0f;
        if (response.doubleClicked && (index >= 0)) {
            if (points.size() > 1) {
                points.erase(std::next(points.begin(), index));
                response.changed = true;
            }
        }
        else if (response.doubleClicked) {
            const auto point = mapping.toCurve(pointer);
            const auto position = std::upper_bound(points.begin(), points.end(), point.x, [](f32 x, const CurvePoint& p) { return x < p.x; });
            const auto inserted = points.insert(position, point);
            dragged = static_cast<f32>(std::distance(points.begin(), inserted) + 1);
            response.changed = true;
        }
        else if (index >= 0) {
            dragged = static_cast<f32>(index + 1);
        }
    }
    else if (response.isHeld() && (dragged > 0.0f)) {
        const auto index = static_cast<std::size_t>(dragged) - 1;
        if (index < points.size()) {
            // NOTE: Keep the point between its neighbors so the curve stays sorted.
            auto point = mapping.toCurve(pointer);
            const auto minX = (index > 0) ? points[index - 1].x : 0.0f;
            const auto maxX = (index + 1 < points.size()) ? points[index + 1].x : 1.0f;
            point.x = std::clamp(point.x, minX, maxX);
            if ((point.x != points[index].x) || (point.y != points[index].y)) {
                points[index] = point;
                response.changed = true;
            }
        }
    }

    i32 hotIndex = -1;
    if (response.isHeld() && (dragged > 0.0f)) {
        hotIndex = static_cast<i32>(dragged) - 1;
    }
    else if (response.hovered) {
        hotIndex = findPointAt(points, mapping, pointer);
    }

    const auto border = detail::getInputBorderColor(style, response, false);
    detail::drawFrame(ui, frame, style.inputBackgroundColor, border, style.frameRounding);

    // NOTE: Grid lines at every quarter of each axis.
    for (i32 i = 1; i < 4; ++i) {
        const auto t = static_cast<f32>(i) * 0.25f;
        const auto x = std::round(plot.x + plot.width * t);
        const auto y = std::round(plot.y + plot.height * t);
        ui.fillRect(F32Rect2D{x, plot.y, 1.0f, plot.height}, style.inputBorderColor);
        ui.fillRect(F32Rect2D{plot.x, y, plot.width, 1.0f}, style.inputBorderColor);
    }

    if (!points.empty()) {
        constexpr f32 lineThickness = 2.0f;
        const auto& first = points.front();
        const auto& last = points.back();
        ui.drawLine(mapping.toScreen(0.0f, first.y), mapping.toScreen(first.x, first.y), style.accentColor, lineThickness);
        for (std::size_t i = 1; i < points.size(); ++i) {
            const auto a = mapping.toScreen(points[i - 1].x, points[i - 1].y);
            const auto b = mapping.toScreen(points[i].x, points[i].y);
            ui.drawLine(a, b, style.accentColor, lineThickness);
        }
        ui.drawLine(mapping.toScreen(last.x, last.y), mapping.toScreen(1.0f, last.y), style.accentColor, lineThickness);

        for (std::size_t i = 0; i < points.size(); ++i) {
            const auto p = mapping.toScreen(points[i].x, points[i].y);
            const bool hot = (static_cast<i32>(i) == hotIndex);
            ui.fillCircle(p, hot ? pointRadiusHot : pointRadius, style.textOnAccentColor);
            ui.fillCircle(p, (hot ? pointRadiusHot : pointRadius) - 1.5f, style.accentColor);
        }
    }

    if ((hotIndex >= 0) && (static_cast<std::size_t>(hotIndex) < points.size())) {
        // NOTE: Show the coordinates of the point under the pointer in the
        // top-right corner of the graph.
        const auto& point = points[static_cast<std::size_t>(hotIndex)];
        detail::NumberText buffer;
        const auto result = fmt::format_to_n(buffer.data(), buffer.size(), "{:.2f}, {:.2f}", point.x, point.y);
        const auto text = std::string_view{buffer.data(), std::min(result.size, buffer.size())};
        const auto textWidth = ui.measureText(text);
        ui.drawText(Vector2{frame.x + frame.width - style.framePadding.x - textWidth, frame.y + 4.0f}, text, style.textSecondaryColor);
    }
    return response;
}

} // namespace pomdog::devgui
