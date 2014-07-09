//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_RENDERER_DD215500_4859_4CB5_886B_2343369AF8BB_HPP
#define POMDOG_RENDERER_DD215500_4859_4CB5_886B_2343369AF8BB_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Math/Matrix4x4.hpp>
#include <Pomdog/Gameplay/Component.hpp>

namespace Pomdog {

class RenderQueue;

class Renderer: public Component<Renderer> {
public:
	virtual ~Renderer() = default;

	virtual void Visit(GameObject & gameObject, RenderQueue & renderQueue, Matrix4x4 const& viewMatrix, Matrix4x4 const& projectionMatrix) = 0;
	
	virtual void SetZOrder(float zOrder) = 0;
	virtual float GetZOrder() const = 0; // SortingOrder
	
	virtual void SetVisible(bool isVisible) = 0;
	virtual bool IsVisible() const = 0;
};

}// namespace Pomdog

#endif // !defined(POMDOG_RENDERER_DD215500_4859_4CB5_886B_2343369AF8BB_HPP)
