//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "ParticleRenderable.hpp"
#include "Pomdog.Experimental/Rendering/Renderer.hpp"
#include "Pomdog.Experimental/Particle2D/ParticleLoader.hpp"
#include "Pomdog.Experimental/Particle2D/ParticleSystem.hpp"

namespace Pomdog {
namespace {

static Matrix3x2 CreateTransformMatrix3x2(Transform2D const& transform)
{
	return Matrix3x2::CreateScale(transform.Scale)
		* Matrix3x2::CreateRotation(transform.Rotation)
		* Matrix3x2::CreateTranslation(transform.Position);
}

}// unnamed namespace
//-----------------------------------------------------------------------
ParticleRenderable::ParticleRenderable(std::shared_ptr<Texture2D> const& texture, std::shared_ptr<BlendState> const& blendState)
{
	command.texture = texture;
	command.textureRegion.Subrect = Rectangle{0, 0, texture->Width(), texture->Height()};
	command.textureRegion.XOffset = 0;
	command.textureRegion.YOffset = 0;
	command.textureRegion.Width = texture->Width();
	command.textureRegion.Height = texture->Height();
	command.textureRegion.Rotate = false;
	command.transform = Matrix3x2::Identity;
	command.blendState = blendState;
	command.particles = nullptr;
}
//-----------------------------------------------------------------------
void ParticleRenderable::Visit(GameObject & gameObject, Renderer & renderer)
{
	if (!IsVisible) {
		return;
	}

	auto particleSystem = gameObject.Component<ParticleSystem>();

	POMDOG_ASSERT(particleSystem);

	if (!particleSystem) {
		return;
	}

	if (particleSystem->Particles().empty()) {
		return;
	}

	command.particles = &particleSystem->Particles();
	command.drawOrder = DrawOrder;

	if (auto transform = gameObject.Component<Transform2D>())
	{
		command.transform = CreateTransformMatrix3x2(*transform);
	}

	renderer.PushCommand(command);
}
//-----------------------------------------------------------------------
}// namespace Pomdog
