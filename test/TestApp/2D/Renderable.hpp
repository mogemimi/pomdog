//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_RENDERABLE_45A0A118_A075_4172_929C_05DF1A2E47FF_HPP
#define POMDOG_RENDERABLE_45A0A118_A075_4172_929C_05DF1A2E47FF_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Math/Matrix4x4.hpp>
#include <Pomdog/Gameplay/Component.hpp>

namespace Pomdog {

class Renderer;

class Renderable: public Component<Renderable> {
public:
	virtual ~Renderable() = default;

	virtual void Visit(GameObject & gameObject, Renderer & renderer, Matrix4x4 const& viewMatrix, Matrix4x4 const& projectionMatrix) = 0;
	
	virtual void ZOrder(float zOrder) = 0;
	virtual float ZOrder() const = 0; // SortingOrder
	
	virtual void IsVisible(bool isVisible) = 0;
	virtual bool IsVisible() const = 0;
};

}// namespace Pomdog

#endif // !defined(POMDOG_RENDERABLE_45A0A118_A075_4172_929C_05DF1A2E47FF_HPP)
