//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_PRIMITIVEGRID_8E899444_553D_4CCC_87EB_11D70DFEC99C_HPP
#define POMDOG_PRIMITIVEGRID_8E899444_553D_4CCC_87EB_11D70DFEC99C_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <vector>
#include <Pomdog/Pomdog.hpp>
#include "LineRenderer.hpp"

namespace TestApp {

using namespace Pomdog;

class PrimitiveGrid {
public:
	PrimitiveGrid(std::shared_ptr<GameHost> const& gameHost,
		Color const& primaryColor, Color const& secondaryColor);
	
	void Draw(GraphicsContext & graphicsContext, Matrix4x4 const& transformMatrix);
	
private:
	struct Line {
		Vector2 Point1;
		Vector2 Point2;
		Color Color;
	};

	std::vector<Line> lines;
	LineRenderer renderer;
};

}// namespace TestApp

#endif // !defined(POMDOG_PRIMITIVEGRID_8E899444_553D_4CCC_87EB_11D70DFEC99C_HPP)
