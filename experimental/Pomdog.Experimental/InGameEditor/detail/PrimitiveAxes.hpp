// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PRIMITIVEAXES_B7717D29_HPP
#define POMDOG_PRIMITIVEAXES_B7717D29_HPP

#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "Pomdog/Math/Color.hpp"
#include <array>

namespace Pomdog {

class LineBatch;

namespace SceneEditor {

class PrimitiveAxes {
public:
    PrimitiveAxes(Color const& xAxisColor, Color const& yAxisColor, Color const& zAxisColor);

    void Draw(LineBatch & lineBatch);

private:
    struct Line {
        Vector3 Point1;
        Vector3 Point2;
        Color Color;
    };

    std::array<Line, 3> lines;
};

}// namespace SceneEditor
}// namespace Pomdog

#endif // POMDOG_PRIMITIVEAXES_B7717D29_HPP
