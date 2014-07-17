//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SPRITERENDERABLE_552FA480_6E30_4845_A7B9_6480CB2E5884_HPP
#define POMDOG_SPRITERENDERABLE_552FA480_6E30_4845_A7B9_6480CB2E5884_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include <Pomdog/Pomdog.hpp>
#include <Pomdog/Math/Rectangle.hpp>
#include <Pomdog/Math/Vector2.hpp>
#include "Renderable.hpp"
#include "../Rendering/SpriteCommand.hpp"

namespace Pomdog {

class SpriteRenderable: public Renderable {
public:
	explicit SpriteRenderable(std::shared_ptr<Texture2D> const& texture);

	SpriteRenderable(std::shared_ptr<Texture2D> const& texture, TextureRegion const& textureRegion);

	void Visit(GameObject & gameObject, Renderer & renderer, Matrix4x4 const& viewMatrix, Matrix4x4 const& projectionMatrix) override;

	void ZOrder(float zOrder) override;
	float ZOrder() const override;
	
	void IsVisible(bool isVisible) override;
	bool IsVisible() const override;

	void Color(Pomdog::Color const& color);
	Pomdog::Color Color() const;

private:
	Details::Rendering::SpriteCommand command;
	float zOrder;
	bool isVisible;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SPRITERENDERABLE_552FA480_6E30_4845_A7B9_6480CB2E5884_HPP)
