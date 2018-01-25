// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "PrimitiveAxes.hpp"
#include "Pomdog.Experimental/Graphics/LineBatch.hpp"

namespace Pomdog {
namespace SceneEditor {

PrimitiveAxes::PrimitiveAxes(Color const& xAxisColor, Color const& yAxisColor, Color const& zAxisColor)
{
    constexpr float axesPixelLength = 4096.0f;

    lines = {
        // x axis
        Vector3(-1.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), xAxisColor,
        // y axis
        Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), yAxisColor,
        // z axis
        Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 0.0f, 1.0f), zAxisColor
    };

    for (auto & line: lines)
    {
        line.Point1 *= axesPixelLength;
        line.Point2 *= axesPixelLength;
    }
}

void PrimitiveAxes::Draw(LineBatch & lineBatch)
{
    for (auto & line: lines)
    {
        lineBatch.DrawLine(line.Point1, line.Point2, line.Color);
    }
}

}// namespace SceneEditor
}// namespace Pomdog
