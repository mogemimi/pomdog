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
#pragma once
#endif

#include <Pomdog/Pomdog.hpp>
#include <typeindex>
#include <memory>

namespace Pomdog {

class RenderCommand;
class RenderCommandProcessor;

class Renderer {
public:
	Renderer(std::shared_ptr<GraphicsContext> const& graphicsContext,
		std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	~Renderer();

	void ViewMatrix(Matrix4x4 const& viewMatrix);
	
	void ProjectionMatrix(Matrix4x4 const& projectionMatrix);

	void Render(GraphicsContext & graphicsContext);
	
	void PushCommand(std::reference_wrapper<RenderCommand> && command);
	
	void Clear();
	
	void AddProcessor(std::type_index const& index, std::unique_ptr<RenderCommandProcessor> && processor);
	
	std::uint32_t DrawCallCount() const;
	
public:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace Pomdog

#endif // !defined(POMDOG_RENDERER_1196526B_9C0E_408B_A501_2A5D941209ED_HPP)
