//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_CAMERAVIEW2D_2AAAB66E_D14C_44F3_9B38_B2B9053963CC_HPP
#define POMDOG_CAMERAVIEW2D_2AAAB66E_D14C_44F3_9B38_B2B9053963CC_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>

namespace TestApp {

using namespace Pomdog;

class CameraView2D
{
public:
	explicit CameraView2D();
	
	void Input(MouseState const& mouseState, Rectangle const& viewportSize, Transform2D & transform, Camera2D & camera);
	
private:
	//std::optional<Vector2> trackStartPosition;
	//std::optional<Vector2> tumbleStartPosition;
	std::unique_ptr<Vector2> trackStartPosition;
	std::unique_ptr<Vector2> tumbleStartPosition;
};

}// namespace TestApp

#endif // !defined(POMDOG_CAMERAVIEW2D_2AAAB66E_D14C_44F3_9B38_B2B9053963CC_HPP)
