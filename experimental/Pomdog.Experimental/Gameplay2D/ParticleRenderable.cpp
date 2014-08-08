//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "ParticleRenderable.hpp"
#include "../Rendering/Renderer.hpp"
#include "Pomdog.Experimental/Graphics/SpriteRenderer.hpp"

///@todo badcode: unnecessary includes
#include "Pomdog.Experimental/Particle2D/ParticleEmitterShapeBox.hpp"
#include "Pomdog.Experimental/Particle2D/ParticleEmitterShapeSector.hpp"
#include "Pomdog.Experimental/Particle2D/ParticleParameterConstant.hpp"
#include "Pomdog.Experimental/Particle2D/ParticleParameterCurve.hpp"
#include "Pomdog.Experimental/Particle2D/ParticleParameterRandom.hpp"
#include "Pomdog.Experimental/Particle2D/ParticleParameterRandomCurves.hpp"

namespace Pomdog {
namespace {
//-----------------------------------------------------------------------
static ParticleEmitter CreateEmitterFireBlock()
{
	ParticleEmitter emitter;

	emitter.MaxParticles = 1024;
	emitter.EmissionRate = 128*2;
	emitter.Duration = DurationSeconds{0.1};
	//emitter.Looping = false;
	emitter.StartLifetime = 1.8f;
	//emitter.GravityModifier = 100.0f;
	
	//emitter.Shape = std::make_unique<ParticleEmitterShapeSector>(MathConstants<float>::PiOver4());
	emitter.Shape = std::make_unique<ParticleEmitterShapeBox>(0, 100);
	
	emitter.StartSpeed = std::make_unique<ParticleParameterRandom<float>>(40.0f, 128.0f);
	//emitter.StartSpeed = std::make_unique<ParticleParameterConstant<float>>(-128.0f);
	
//	emitter.StartSpeed = std::make_unique<ParticleParameterCurve<float>>(
//		std::initializer_list<ParticleCurveKey<float>>{
//			{0.00f, 0.0f},
//			{0.10f, -0.5f},
//			{0.15f, -1.0f},
//			{0.20f, -0.5f},
//			{0.40f, 0.0f},
//			{0.50f, 0.5f},
//			{0.70f, 1.0f},
//			{0.80f, 0.5f},
//			{1.00f, 0.0f},
//		});
	
	emitter.StartColor = std::make_unique<ParticleParameterConstant<Color>>(Color::White);
	//emitter.StartColor = std::make_unique<ParticleParameterConstant<Color>>(Color::White);
	//emitter.StartColor = std::make_unique<ParticleParameterRandom<Color>>(Color::Black, Color::White);
	
	//emitter.ColorOverLifetime = std::make_unique<ParticleParameterConstant<Color>>(Color::White);
	//emitter.ColorOverLifetime = std::make_unique<ParticleParameterRandom<Color>>(Color::Yellow, Color::Black);
	emitter.ColorOverLifetime = std::make_unique<ParticleParameterCurve<Color>>(
		std::initializer_list<ParticleCurveKey<Color>>{
			{0.00f, Color{255, 255, 255, 0}},
			{0.02f, Color{255, 255, 255, 10}},
			{0.09f, Color{255, 250, 180, 100}},
			{0.15f, Color{255, 200, 180, 130}},
			{0.19f, Color{200, 130, 60, 255}},
			{0.24f, Color{190, 50, 10, 80}},
			{0.32f, Color{80, 24, 2, 20}},
			{1.00f, Color{0, 0, 0, 0}},
		});
	
	//emitter.StartRotation = std::make_unique<ParticleParameterConstant<Radian<float>>>(0);
	emitter.StartRotation = std::make_unique<ParticleParameterRandom<Radian<float>>>(
		0, MathConstants<float>::TwoPi());
	
	//emitter.RotationOverLifetime = std::make_unique<ParticleParameterConstant<Radian<float>>>(0);
	emitter.RotationOverLifetime = std::make_unique<ParticleParameterRandom<Radian<float>>>(
		-MathConstants<float>::PiOver4(), MathConstants<float>::PiOver4());
	
	//emitter.StartSize = std::make_unique<ParticleParameterConstant<float>>(1.0f);
	emitter.StartSize = std::make_unique<ParticleParameterRandom<float>>(0.8f, 1.2f);
	
	//emitter.SizeOverLifetime = std::make_unique<ParticleParameterConstant<float>>(1.0f);
	emitter.SizeOverLifetime = std::make_unique<ParticleParameterCurve<float>>(
		std::initializer_list<ParticleCurveKey<float>>{
			{0.00f, 0.0f},
			{0.03f, 0.5f},
			{0.10f, 0.8f},
			{0.15f, 1.0f},
			{0.60f, 0.8f},
			{1.00f, 0.0f},
		});

	return std::move(emitter);
}

}// unnamed namespace
//-----------------------------------------------------------------------
ParticleRenderable::ParticleRenderable()
{}
//-----------------------------------------------------------------------
void ParticleRenderable::Visit(GameObject & gameObject, Renderer & renderer,
	Matrix4x4 const& viewMatrix, Matrix4x4 const& projectionMatrix)
{
	if (!IsVisible) {
		return;
	}
	
	if (particleSystem.particles.empty()) {
		return;
	}
	
	command.drawOrder = DrawOrder;
	command.spriteRenderer->SetProjectionMatrix(projectionMatrix);
	command.modelViewProjection = viewMatrix;
	
//	if (auto transform = gameObject.Component<Transform2D>())
//	{
//		command.modelViewProjection = CreateTransformMatrix4x4(*transform)
//			* command.modelViewProjection;
//	}
	
	renderer.PushCommand(command);
}
//-----------------------------------------------------------------------
void ParticleRenderable::Load(std::shared_ptr<GraphicsContext> const& graphicsContext,
	std::shared_ptr<GraphicsDevice> const& graphicsDevice, AssetManager & assets)
{
	command.texture = assets.Load<Texture2D>("Particles/smoke.png");
	command.particles = &particleSystem.particles;
	command.spriteRenderer = std::make_unique<SpriteRenderer>(graphicsContext, graphicsDevice);
	//command.blendState = BlendState::CreateAdditive(graphicsDevice);
	command.blendState = BlendState::CreateNonPremultiplied(graphicsDevice);

	// for test
	particleSystem.emitter = CreateEmitterFireBlock();
}
//-----------------------------------------------------------------------
void ParticleRenderable::Update(GameObject & object, GameClock const& clock)
{
	if (auto transform = object.Component<Transform2D>())
	{
		particleSystem.Update(clock.FrameDuration(), *transform);
	}
}
//-----------------------------------------------------------------------
}// namespace Pomdog
