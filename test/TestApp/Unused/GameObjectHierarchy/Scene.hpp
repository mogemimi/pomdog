// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SCENE_B27EF22F_35AA_4103_9523_D05577E7E36F_HPP
#define POMDOG_SCENE_B27EF22F_35AA_4103_9523_D05577E7E36F_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <memory>
#include "SceneNode.hpp"

namespace Pomdog {

class Scene final: public SceneNode {
public:
	explicit Scene();
	
	void Update(DurationSeconds const& frameDuration) override;

	void Draw() override;

private:
};

}// namespace Pomdog

#endif // !defined(POMDOG_SCENE_B27EF22F_35AA_4103_9523_D05577E7E36F_HPP)
