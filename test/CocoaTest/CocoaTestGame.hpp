//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_COCOATESTGAME_HPP
#define POMDOG_COCOATESTGAME_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

namespace Pomdog {

class CocoaTestGame
{
public:
	void Initialize();
	
	void Update();
	
	void Draw();
};

}// namespace Pomdog

#endif // !defined(POMDOG_COCOATESTGAME_HPP)
