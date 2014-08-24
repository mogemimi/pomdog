//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SPRITECOMMAND_8EB48EFB_54D7_4AF1_AB24_B8607C44B642_HPP
#define POMDOG_SPRITECOMMAND_8EB48EFB_54D7_4AF1_AB24_B8607C44B642_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "RenderCommand.hpp"
#include "Pomdog.Experimental/Graphics/TextureRegion.hpp"
#include "Pomdog.Experimental/Graphics/SpriteRenderer.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {
namespace Details {
namespace Rendering {

class SpriteCommand final: public RenderCommand {
public:
	float DrawOrder() const override
	{
		return drawOrder;
	}

	void Execute(GraphicsContext & graphicsContext) override;

	RenderCommandType CommandType() const override;

public:
	Matrix3x2 transform;
	TextureRegion textureRegion;
	std::shared_ptr<Texture2D> texture;
	Vector2 originPivot;
	Color color;
	float drawOrder;
};

}// namespace Rendering
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SPRITECOMMAND_8EB48EFB_54D7_4AF1_AB24_B8607C44B642_HPP)
