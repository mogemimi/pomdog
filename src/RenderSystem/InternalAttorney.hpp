//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_INTERNALATTORNEY_HPP
#define POMDOG_SRC_RENDERSYSTEM_INTERNALATTORNEY_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Graphics/GraphicsContext.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class InternalAttorney
{
public:
	static NativeGraphicsContext* Native(GraphicsContext & graphicsContext)
	{
		return graphicsContext.NativeContext();
	}
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_INTERNALATTORNEY_HPP)
