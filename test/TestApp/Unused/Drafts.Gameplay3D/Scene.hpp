//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SCENE_CA9897EE_0207_4645_A138_AD92A23D1207_HPP
#define POMDOG_SCENE_CA9897EE_0207_4645_A138_AD92A23D1207_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <memory>
#include <Pomdog/Utility/Noncopyable.hpp>

namespace Pomdog {

class GraphicsContext;
class RenderNode;

class Scene final: Noncopyable
{
public:
	explicit Scene(std::shared_ptr<GraphicsContext> const& graphicsContext);

	~Scene();

	void AddChild(std::shared_ptr<RenderNode> const& node);
	void AddChild(std::shared_ptr<RenderNode> && node);

	void RemoveChild(std::shared_ptr<RenderNode> const& node);

	void Render();

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SCENE_CA9897EE_0207_4645_A138_AD92A23D1207_HPP)
