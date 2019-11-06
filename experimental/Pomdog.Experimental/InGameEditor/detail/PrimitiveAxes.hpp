// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include <array>

namespace Pomdog {

class LineBatch;

namespace SceneEditor {

class PrimitiveAxes {
public:
    PrimitiveAxes(const Color& xAxisColor, const Color& yAxisColor, const Color& zAxisColor);

    void Draw(LineBatch& lineBatch);

private:
    struct Line {
        Vector3 Point1;
        Vector3 Point2;
        Color Color;
    };

    std::array<Line, 3> lines;
};

} // namespace SceneEditor
} // namespace Pomdog
