//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "Renderer.hpp"
#include "RenderCommand.hpp"

namespace Pomdog {

void Renderer::Render(std::shared_ptr<GraphicsContext> const& graphicsContext)
{
	renderQueue.Sort();
	renderQueue.Enumerate([&](RenderCommand & command)
	{
		command.Execute(graphicsContext);
	});
	renderQueue.Clear();
}

}// namespace Pomdog
