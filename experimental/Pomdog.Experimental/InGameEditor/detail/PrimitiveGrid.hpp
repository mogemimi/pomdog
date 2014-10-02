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
#include <Pomdog/Math/Vector2.hpp>
#include <Pomdog/Math/Matrix4x4.hpp>
#include <Pomdog/Math/Color.hpp>
#include <Pomdog/Pomdog.hpp>

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
