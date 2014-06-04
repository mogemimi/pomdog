//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "PrimitiveGrid.hpp"

namespace TestApp {
//-----------------------------------------------------------------------
PrimitiveGrid::PrimitiveGrid(std::shared_ptr<GameHost> const& gameHost,
	Color const& primaryColor, Color const& secondaryColor)
	: renderer(gameHost)
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
void PrimitiveGrid::Draw(GraphicsContext & graphicsContext, Matrix4x4 const& transformMatrix)
{
	renderer.Begin(transformMatrix);
	for (auto & line: lines)
	{
		renderer.Draw(line.Point1, line.Point2, line.Color);
	}
	renderer.End();
}

}// namespace TestApp
