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

#include "Renderable.hpp"
#include "Pomdog.Experimental/Rendering/Commands/SpriteCommand.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {

class SpriteRenderable: public Renderable {
public:
	explicit SpriteRenderable(std::shared_ptr<Texture2D> const& texture);

	SpriteRenderable(std::shared_ptr<Texture2D> const& texture, TextureRegion const& textureRegion);

	void Visit(GameObject & gameObject, Renderer & renderer, Matrix4x4 const& viewMatrix, Matrix4x4 const& projectionMatrix) override;

	void OriginPivot(Vector2 const& originPivot);
	Vector2 OriginPivot() const;

	void Region(TextureRegion const& region);
	TextureRegion const& Region() const;
	
	Rectangle BoundingBox() const;

private:
	Details::Rendering::SpriteCommand command;
	Matrix3x2 offsetMatrix;
	Vector2 originPivot;
	std::uint32_t dirtyFlags;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SPRITERENDERABLE_552FA480_6E30_4845_A7B9_6480CB2E5884_HPP)
