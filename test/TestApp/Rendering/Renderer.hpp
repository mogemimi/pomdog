//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_RENDERER_1196526B_9C0E_408B_A501_2A5D941209ED_HPP
#define POMDOG_RENDERER_1196526B_9C0E_408B_A501_2A5D941209ED_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>
#include "RenderQueue.hpp"

namespace Pomdog {

class Renderer {
public:
	void Render(std::shared_ptr<GraphicsContext> const& graphicsContext);
		
public:
	RenderQueue renderQueue;
};

}// namespace Pomdog

#endif // !defined(POMDOG_RENDERER_1196526B_9C0E_408B_A501_2A5D941209ED_HPP)
