//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "ParticleSystem.hpp"
#include <random>

namespace Pomdog {
namespace {
//-----------------------------------------------------------------------
template <typename RandomGenerator>
static Particle CreateParticle(RandomGenerator & random, ParticleEmitter const& emitter, Vector2 const& emitterPosition)
{
	Particle particle;
	
	particle.TimeToLive = emitter.StartLifetime;
	
	// Position
	particle.Position = emitterPosition;
	
	std::uniform_real_distribution<float> distribution(-1.0, 1.0);
	
	// Velocity
	particle.Velocity.X = distribution(random) *1.0f;
	particle.Velocity.Y = distribution(random) *1.0f;
	
	// Rotation
	particle.Rotation = distribution(random) * MathConstants<float>::PiOver2();
	
	// Color
	particle.Color = emitter.StartColor;
	
	return std::move(particle);
}
//-----------------------------------------------------------------------
}// unnamed namespace
//-----------------------------------------------------------------------
ParticleSystem::ParticleSystem()
	: timer(0)
{
	particles.reserve(emitter.MaxParticles);
	emitter.MaxParticles = 1024;
	emitter.EmissionRate = 128*2;
	emitter.StartLifetime = 1.8;
	//emitter.EmissionRate = 2;
}
//-----------------------------------------------------------------------
void ParticleSystem::Update(DurationSeconds const& frameDuration)
{
	{
		timer += frameDuration;
		
		POMDOG_ASSERT(emitter.EmissionRate > 0);
		auto emissionInterval = std::max(std::numeric_limits<DurationSeconds>::epsilon(),
			DurationSeconds(1) / emitter.EmissionRate);
		
		POMDOG_ASSERT(emissionInterval.count() > 0);
		
		while ((particles.size() < emitter.MaxParticles) && (timer >= emissionInterval))
		{
			timer -= emissionInterval;
			
			///@todo need to refactor code
			static std::mt19937 random(10000);
			
			auto particle = CreateParticle(random, emitter, EmitterPosition);
			particles.push_back(std::move(particle));
		}
		
		ParticleCurve<Color> colorCurve({
			{0.00f, Color{255, 255, 255, 0}},
			{0.02f, Color{255, 255, 255, 10}},
			{0.09f, Color{255, 250, 180, 100}},
			{0.15f, Color{255, 200, 180, 130}},
			{0.22f, Color{200, 140, 60, 255}},
			{0.28f, Color{170, 120, 10, 80}},
			{0.32f, Color{80, 40, 10, 20}},
			{1.00f, Color{0, 0, 0, 0}},
		});
		
		//ParticleCurve<Color> colorCurve(Color::White);
		
		ParticleCurve<float> scaleCurve({
			{0.00f, 0.8f},
			{0.10f, 1.0f},
			{0.15f, 1.1f},
			{0.60f, 0.8f},
			{1.00f, 0.0f},
		});

		
		for (auto & particle: particles)
		{
			particle.TimeToLive -= frameDuration.count();
		
			particle.Position += particle.Velocity;
			
			float curvePosition = 1.0f - particle.TimeToLive/emitter.StartLifetime;
			
			particle.Color = colorCurve.Compute(curvePosition);
			particle.Size = scaleCurve.Compute(curvePosition);
		}
		
		particles.erase(std::remove_if(std::begin(particles), std::end(particles),
			[](Particle const& p){ return p.TimeToLive <= 0; }), std::end(particles));
	}
}
//-----------------------------------------------------------------------
}// namespace Pomdog
