//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_PRIMITIVEAXES_B7717D29_25D2_44E7_8F48_2642088D2B41_HPP
#define POMDOG_PRIMITIVEAXES_B7717D29_25D2_44E7_8F48_2642088D2B41_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <array>
#include <Pomdog/Math/Vector2.hpp>
#include <Pomdog/Math/Vector3.hpp>
#include <Pomdog/Math/Matrix4x4.hpp>
#include <Pomdog/Math/Color.hpp>
#include <Pomdog/Pomdog.hpp>

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

#endif // !defined(POMDOG_PRIMITIVEAXES_B7717D29_25D2_44E7_8F48_2642088D2B41_HPP)
