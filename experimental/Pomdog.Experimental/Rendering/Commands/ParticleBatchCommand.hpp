// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PARTICLEBATCHCOMMAND_BBF33260_CA29_4B07_BBEB_8C19916FF5CC_HPP
#define POMDOG_PARTICLEBATCHCOMMAND_BBF33260_CA29_4B07_BBEB_8C19916FF5CC_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog.Experimental/Rendering/RenderCommand.hpp"
#include "Pomdog.Experimental/Graphics/TextureRegion.hpp"
#include "Pomdog.Experimental/Particle2D/Particle.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {

class SpriteRenderer;

namespace Detail {
namespace Rendering {

class ParticleBatchCommand final: public RenderCommand {
public:
	float DrawOrder() const override
	{
		return drawOrder;
	}

	std::type_index TypeIndex() const override;

public:
	TextureRegion textureRegion;
	Matrix3x2 transform;
	std::shared_ptr<BlendState> blendState;
	std::shared_ptr<Texture2D> texture;
	std::vector<Particle> const* particles = nullptr;
	float drawOrder;
};

}// namespace Rendering
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLEBATCHCOMMAND_BBF33260_CA29_4B07_BBEB_8C19916FF5CC_HPP)
