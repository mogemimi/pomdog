// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#include "ParticleBatchCommandProcessor.hpp"
#include "Pomdog.Experimental/Rendering/Commands/ParticleBatchCommand.hpp"

namespace Pomdog {
namespace {

Matrix3x2 CreateTransformMatrix(const Particle& particle)
{
    return Matrix3x2::CreateScale(particle.Size)
        * Matrix3x2::CreateRotation(particle.Rotation)
        * Matrix3x2::CreateTranslation(particle.Position);
}

} // unnamed namespace

ParticleBatchCommandProcessor::ParticleBatchCommandProcessor(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    AssetManager & assets)
    : spriteBatch(graphicsDevice, assets)
    , drawCallCount(0)
{
}

void ParticleBatchCommandProcessor::Begin(GraphicsCommandQueue &)
{
    drawCallCount = 0;
    spriteBatch.Begin(Matrix4x4::Identity);
}

void ParticleBatchCommandProcessor::Draw(GraphicsCommandQueue & commandQueue, RenderCommand & command)
{
    using Detail::Rendering::ParticleBatchCommand;

    auto & particleCommand = static_cast<ParticleBatchCommand &>(command);
    for (auto & particle: *particleCommand.particles)
    {
        auto transform = CreateTransformMatrix(particle);
        spriteBatch.Draw(
            particleCommand.texture,
            transform,
            particleCommand.textureRegion.Subrect,
            particle.Color,
            {0.5f, 0.5f});
    }
}

void ParticleBatchCommandProcessor::End(GraphicsCommandQueue & commandQueue)
{
    spriteBatch.End();

    drawCallCount += spriteBatch.DrawCallCount();
}

int ParticleBatchCommandProcessor::GetDrawCallCount() const noexcept
{
    return drawCallCount;
}

void ParticleBatchCommandProcessor::SetViewProjection(const Matrix4x4& view, const Matrix4x4& projection)
{
    auto viewProjection = view * projection;
    spriteBatch.SetProjectionMatrix(viewProjection);
}

} // namespace Pomdog
