// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#include "PrimitiveGrid.hpp"
#include "Pomdog.Experimental/Graphics/LineBatch.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace SceneEditor {
//-----------------------------------------------------------------------
PrimitiveGrid::PrimitiveGrid(Color const& primaryColor, Color const& secondaryColor)
{
    constexpr std::uint32_t gridCount = 30;
    constexpr float gridPixelSize = 96.0f;

    lines.reserve((1 + gridCount*2) * 2);

    POMDOG_ASSERT(gridCount > 0);
    auto const lineLength = gridPixelSize * (gridCount);

    for (std::size_t i = 0; i < (1 + 2 * gridCount); ++i)
    {
        auto lineNumber = ((i + 1) / 2);
        auto offset = (gridPixelSize * lineNumber) * ((i % 2 == 0) ? 1.0f: -1.0f);

        Color color = (lineNumber % 10 == 0) ? primaryColor: secondaryColor;

        lines.push_back({Vector2{-lineLength, offset}, Vector2{+lineLength, offset}, color});
        lines.push_back({Vector2{offset, -lineLength}, Vector2{offset, +lineLength}, color});
    }
}
//-----------------------------------------------------------------------
void PrimitiveGrid::Draw(LineBatch & lineBatch)
{
    for (auto & line: lines)
    {
        lineBatch.DrawLine(line.Point1, line.Point2, line.Color);
    }
}

}// namespace SceneEditor
}// namespace Pomdog
