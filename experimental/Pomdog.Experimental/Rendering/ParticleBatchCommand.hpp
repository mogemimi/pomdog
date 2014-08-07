//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_PARTICLEBATCHCOMMAND_BBF33260_CA29_4B07_BBEB_8C19916FF5CC_HPP
#define POMDOG_PARTICLEBATCHCOMMAND_BBF33260_CA29_4B07_BBEB_8C19916FF5CC_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "RenderCommand.hpp"
#include <Pomdog.Experimental/Particle2D/Particle.hpp>
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {

class SpriteRenderer;

namespace Details {
namespace Rendering {

class ParticleBatchCommand final: public RenderCommand {
public:
	~ParticleBatchCommand();

	float DrawOrder() const override
	{
		return drawOrder;
	}

	RenderCommandType CommandType() const override;
	
	void Execute(std::shared_ptr<GraphicsContext> const& graphicsContext) override;

public:
	std::unique_ptr<SpriteRenderer> spriteRenderer;
	std::shared_ptr<BlendState> blendState;
	Matrix4x4 modelViewProjection;
	std::shared_ptr<Texture2D> texture;
	std::vector<Particle> const* particles = nullptr;
	float drawOrder;
};

}// namespace Rendering
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLEBATCHCOMMAND_BBF33260_CA29_4B07_BBEB_8C19916FF5CC_HPP)
