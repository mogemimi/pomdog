//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "ParticleBatchCommandProcessor.hpp"
#include "Pomdog.Experimental/Rendering/Commands/ParticleBatchCommand.hpp"

namespace Pomdog {
namespace {

static Matrix3x2 CreateTransformMatrix(Particle const& particle)
{
	return Matrix3x2::CreateScale(particle.Size)
		* Matrix3x2::CreateRotation(particle.Rotation)
		* Matrix3x2::CreateTranslation(particle.Position);
}

}// unnamed namespace
//-----------------------------------------------------------------------
ParticleBatchCommandProcessor::ParticleBatchCommandProcessor(std::shared_ptr<GraphicsContext> const& graphicsContext,
	std::shared_ptr<GraphicsDevice> const& graphicsDevice)
	: spriteBatch(graphicsContext, graphicsDevice)
	, drawCallCount(0)
{
}
//-----------------------------------------------------------------------
void ParticleBatchCommandProcessor::Begin(GraphicsContext & graphicsContext)
{
	drawCallCount = 0;
	
	spriteBatch.Begin(Matrix4x4::Identity);
}
//-----------------------------------------------------------------------
void ParticleBatchCommandProcessor::Draw(GraphicsContext & graphicsContext, RenderCommand & command)
{
	using Details::Rendering::ParticleBatchCommand;
	
	auto & particleCommand = static_cast<ParticleBatchCommand &>(command);
	for (auto & particle: *particleCommand.particles)
	{
		auto transform = CreateTransformMatrix(particle);
		spriteBatch.Draw(particleCommand.texture, transform,
			particleCommand.textureRegion.Subrect, particle.Color, {0.5f, 0.5f});
	}
}
//-----------------------------------------------------------------------
void ParticleBatchCommandProcessor::End(GraphicsContext & graphicsContext)
{
	spriteBatch.End();

	drawCallCount += spriteBatch.DrawCallCount();
}
//-----------------------------------------------------------------------
int ParticleBatchCommandProcessor::DrawCallCount() const
{
	return drawCallCount;
}
//-----------------------------------------------------------------------
void ParticleBatchCommandProcessor::SetViewProjection(Matrix4x4 const& view, Matrix4x4 const& projection)
{
	auto viewProjection = view * projection;
	spriteBatch.SetProjectionMatrix(viewProjection);
}
//-----------------------------------------------------------------------
}// namespace Pomdog
