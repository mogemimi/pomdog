//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_JOINTPOSE_160B1EF6_234E_4107_ACA9_10FA3189A67C_HPP
#define POMDOG_JOINTPOSE_160B1EF6_234E_4107_ACA9_10FA3189A67C_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Math/Vector2.hpp>
#include <Pomdog/Math/Radian.hpp>

namespace Pomdog {

///@~Japanese
/// @note スケーリングが一様であること（Vector2 ではなく float のスカラー量になっていること）に注意してください。
/// スケーリングが一様なのは、パフォーマンス上の制約が理由になります。
class JointPose {
public:
	Vector2 Translate;
	Radian<float> Rotation;
	float Scale;
};

}// namespace Pomdog

#endif // !defined(POMDOG_JOINTPOSE_160B1EF6_234E_4107_ACA9_10FA3189A67C_HPP)
