// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/devgui/key.h"
#include "pomdog/experimental/devgui/response.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
class UI;
} // namespace pomdog::devgui

namespace pomdog::devgui {

/// Holds one point of a curve edited by `curveEditor()`.
struct CurvePoint final {
    /// The position along the curve, in [0, 1].
    f32 x = 0.0f;

    /// The value at x.
    f32 y = 0.0f;
};

/// Describes the range and size of a curve editor.
struct CurveEditorOptions final {
    /// The value at the bottom of the graph.
    f32 minY = 0.0f;

    /// The value at the top of the graph.
    f32 maxY = 1.0f;

    /// The height of the graph in logical pixels.
    f32 height = 120.0f;
};

/// Displays a labeled graph that edits a piecewise linear curve over x in
/// [0, 1], such as a particle property over its lifetime.
///
/// points must be sorted by x. Dragging a point moves it between its
/// neighbors, double-clicking empty space adds a point there, and
/// double-clicking a point removes it unless it is the last one. Before the
/// first point and after the last, the curve holds their values.
/// `Response::changed` reports frames where points changed.
Response
curveEditor(UI& ui, Key key, std::string_view label, std::vector<CurvePoint>& points, const CurveEditorOptions& options = {});

} // namespace pomdog::devgui
