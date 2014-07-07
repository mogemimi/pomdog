//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MAIDNODE_70588F82_1C97_4E1C_9CCD_EB5D264CFD22_HPP
#define POMDOG_MAIDNODE_70588F82_1C97_4E1C_9CCD_EB5D264CFD22_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../2D/SceneNode.hpp"

namespace TestApp {

using namespace Pomdog;

class MaidNode final: public SceneNode {
public:
	void Update(DurationSeconds const& frameDuration) override;

	void Draw() override;
};

}// namespace TestApp

#endif // !defined(POMDOG_MAIDNODE_70588F82_1C97_4E1C_9CCD_EB5D264CFD22_HPP)
