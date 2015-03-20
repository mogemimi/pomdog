// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_CAMERANODE_6C0263F9_8E18_46DA_B679_76CEF633448D_HPP
#define POMDOG_CAMERANODE_6C0263F9_8E18_46DA_B679_76CEF633448D_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "../2D/SceneNode.hpp"

namespace TestApp {

using namespace Pomdog;

class CameraNode final: public SceneNode {
public:
	void Update(DurationSeconds const& frameDuration) override;

	void Draw() override;
};

}// namespace TestApp

#endif // !defined(POMDOG_CAMERANODE_6C0263F9_8E18_46DA_B679_76CEF633448D_HPP)
