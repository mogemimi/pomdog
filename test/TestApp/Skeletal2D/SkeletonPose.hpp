//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SKELETONPOSE_A5B94631_577E_4148_A8F7_DEBCE99A0218_HPP
#define POMDOG_SKELETONPOSE_A5B94631_577E_4148_A8F7_DEBCE99A0218_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <vector>
#include <Pomdog/Math/Matrix4x4.hpp>
#include "JointPose.hpp"

namespace Pomdog {

class SkeletonPose {
public:
	std::vector<JointPose> LocalPose;
	
	///@note Matrix palette
	std::vector<Matrix4x4> GlobalPose;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SKELETONPOSE_A5B94631_577E_4148_A8F7_DEBCE99A0218_HPP)
