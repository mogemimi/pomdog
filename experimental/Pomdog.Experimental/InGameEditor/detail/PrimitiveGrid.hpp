//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_PRIMITIVEGRID_8E899444_553D_4CCC_87EB_11D70DFEC99C_HPP
#define POMDOG_PRIMITIVEGRID_8E899444_553D_4CCC_87EB_11D70DFEC99C_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Color.hpp"
#include <vector>

namespace Pomdog {

class LineBatch;

namespace SceneEditor {

class PrimitiveGrid {
public:
	PrimitiveGrid(Color const& primaryColor, Color const& secondaryColor);

	void Draw(LineBatch & lineBatch);

private:
	struct Line {
		Vector2 Point1;
		Vector2 Point2;
		Color Color;
	};

	std::vector<Line> lines;
};

}// namespace SceneEditor
}// namespace Pomdog

#endif // !defined(POMDOG_PRIMITIVEGRID_8E899444_553D_4CCC_87EB_11D70DFEC99C_HPP)
