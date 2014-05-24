//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "ParticleSystem.hpp"
#include <random>
#include <Pomdog/Utility/MakeUnique.hpp>

///@note for test
#include "ParticleParameterConstant.hpp"
#include "ParticleParameterCurve.hpp"
#include "ParticleParameterRandom.hpp"

namespace Pomdog {
namespace {
//-----------------------------------------------------------------------
template <typename RandomGenerator>
static Particle CreateParticle(RandomGenerator & random, ParticleEmitter const& emitter, float normalizedTime, Vector2 const& emitterPosition)
{
	Particle particle;
	
	particle.TimeToLive = emitter.StartLifetime;
	
	// Position
	particle.Position = emitterPosition;
	
	// Velocity
	POMDOG_ASSERT(emitter.StartSpeed);
	particle.Velocity.X = emitter.StartSpeed->Compute(normalizedTime, random);
	particle.Velocity.Y = emitter.StartSpeed->Compute(normalizedTime, random);
	
	std::uniform_real_distribution<float> distribution(-1.0, 1.0);
	
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
	: erapsedTime(0)
	, emissionTimer(0)
{
	particles.reserve(emitter.MaxParticles);
	emitter.MaxParticles = 1024;
	emitter.EmissionRate = 128*2;
	emitter.StartLifetime = 1.8;
	//emitter.Duration = 1;
	//emitter.Looping = false;
	//emitter.EmissionRate = 2;
	
	emitter.StartSpeed = Details::MakeUnique<ParticleParameterRandom<float>>(0.5f, 6.0f);
	
//	emitter.StartSpeed = Details::MakeUnique<ParticleParameterCurve<float>>(
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
	
	emitter.ColorOverLifetime = Details::MakeUnique<ParticleParameterCurve<Color>>(
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
	
	emitter.SizeOverLifetime = Details::MakeUnique<ParticleParameterCurve<float>>(
		std::initializer_list<ParticleCurveKey<float>>{
			{0.00f, 0.7f},
			{0.10f, 0.9f},
			{0.15f, 1.0f},
			{0.60f, 0.8f},
			{1.00f, 0.0f},
		});
}
//-----------------------------------------------------------------------
void ParticleSystem::Update(DurationSeconds const& frameDuration)
{
	erapsedTime += frameDuration;
	
	if (emitter.Looping && erapsedTime > emitter.Duration)
	{
		erapsedTime = DurationSeconds{0};
	}

	///@todo need to refactor code
	static std::mt19937 random(10000);

	if (emitter.Looping || erapsedTime <= emitter.Duration)
	{
		emissionTimer += frameDuration;
		
		POMDOG_ASSERT(emitter.EmissionRate > 0);
		auto emissionInterval = std::max(std::numeric_limits<DurationSeconds>::epsilon(),
			DurationSeconds(1) / emitter.EmissionRate);
		
		POMDOG_ASSERT(emissionInterval.count() > 0);

		POMDOG_ASSERT(emitter.Duration.count() > 0);
		float normalizedTime = erapsedTime/emitter.Duration;
		
		while ((particles.size() < emitter.MaxParticles) && (emissionTimer >= emissionInterval))
		{
			emissionTimer -= emissionInterval;

			auto particle = CreateParticle(random, emitter, normalizedTime, EmitterPosition);
			particles.push_back(std::move(particle));
		}
	}
	{
		for (auto & particle: particles)
		{
			particle.TimeToLive -= frameDuration.count();
		
			particle.Position += particle.Velocity;
			
			float normalizedTime = 1.0f - particle.TimeToLive/emitter.StartLifetime;
			
			POMDOG_ASSERT(emitter.ColorOverLifetime);
			POMDOG_ASSERT(emitter.SizeOverLifetime);
			
			particle.Color = emitter.ColorOverLifetime->Compute(normalizedTime, random);
			particle.Size = emitter.SizeOverLifetime->Compute(normalizedTime, random);
		}
		
		particles.erase(std::remove_if(std::begin(particles), std::end(particles),
			[](Particle const& p){ return p.TimeToLive <= 0; }), std::end(particles));
	}
}
//-----------------------------------------------------------------------
}// namespace Pomdog
