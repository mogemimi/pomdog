//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_GL4_INPUTLAYOUTGL4_F0AF2367_CC09_4F30_B3CD_E1B0A3BE5C22_HPP
#define POMDOG_SRC_GL4_INPUTLAYOUTGL4_F0AF2367_CC09_4F30_B3CD_E1B0A3BE5C22_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

class InputLayoutGL4
{
public:
	InputLayoutGL4() = delete;
	
	//InputLayoutGL4();
	
	~InputLayoutGL4();

	void Apply();
	
private:
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_GL4_INPUTLAYOUTGL4_F0AF2367_CC09_4F30_B3CD_E1B0A3BE5C22_HPP)
