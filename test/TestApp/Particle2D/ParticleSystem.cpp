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

namespace Pomdog {
namespace {
//-----------------------------------------------------------------------
template <typename RandomGenerator>
static Particle CreateParticle(RandomGenerator & random, ParticleEmitter const& emitter, float normalizedTime, Transform2D const& transform)
{
	Particle particle;
	{
		particle.TimeToLive = emitter.StartLifetime;
	}
	{
		// emitter shape
		Vector2 emitPosition = Vector2::Zero;
		Radian<float> emitAngle = 0.0f;

		POMDOG_ASSERT(emitter.Shape);
		emitter.Shape->Compute(random, emitPosition, emitAngle);
		
		// Position
		auto worldMatrix = Matrix4x4::CreateRotationZ(transform.Rotation) * Matrix4x4::CreateTranslation({transform.Position, 0});
		particle.Position = Vector2::Transform(emitPosition, worldMatrix);
		
		// Velocity
		POMDOG_ASSERT(emitter.StartSpeed);
		auto radius = emitter.StartSpeed->Compute(normalizedTime, random);
		auto angle = (emitAngle + transform.Rotation).value;
		
		particle.Velocity.X = std::cos(angle) * radius;
		particle.Velocity.Y = std::sin(angle) * radius;
	}
	{
		// Rotation
		POMDOG_ASSERT(emitter.StartRotation);
		particle.Rotation = emitter.StartRotation->Compute(normalizedTime, random);
	}
	{
		// Color
		POMDOG_ASSERT(emitter.StartColor);
		particle.StartColor = emitter.StartColor->Compute(normalizedTime, random);
		particle.Color = particle.StartColor;
	}
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
}
//-----------------------------------------------------------------------
void ParticleSystem::Update(DurationSeconds const& frameDuration, Transform2D const& emitterTransform)
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

			auto particle = CreateParticle(random, emitter, normalizedTime, emitterTransform);
			particles.push_back(std::move(particle));
		}
	}
	{
		for (auto & particle: particles)
		{
			auto oldTimeToLive = particle.TimeToLive;
			particle.TimeToLive -= frameDuration.count();
			
			auto deltaTime = oldTimeToLive - particle.TimeToLive;
			
			Vector2 particleAcceleration {1, 1};
			Vector2 gravityAcceleration {0, -emitter.GravityModifier};
			particle.Velocity += (particleAcceleration * gravityAcceleration * deltaTime);
			particle.Position += (particle.Velocity * deltaTime);
			
			float normalizedTime = 1.0f - particle.TimeToLive/emitter.StartLifetime;
					
			POMDOG_ASSERT(emitter.ColorOverLifetime);
			POMDOG_ASSERT(emitter.SizeOverLifetime);
			
			auto ColorMultiply = [](Color const& a, Color const& b)->Color {
				return {
					static_cast<uint8_t>(MathHelper::Clamp((a.R()/255.0f) * b.R(), 0.0f, 255.0f)),
					static_cast<uint8_t>(MathHelper::Clamp((a.G()/255.0f) * b.G(), 0.0f, 255.0f)),
					static_cast<uint8_t>(MathHelper::Clamp((a.B()/255.0f) * b.B(), 0.0f, 255.0f)),
					static_cast<uint8_t>(MathHelper::Clamp((a.A()/255.0f) * b.A(), 0.0f, 255.0f))};
			};
			//particle.Color = ColorMultiply(particle.StartColor, emitter.ColorOverLifetime->ComputeOnTime(normalizedTime, particle.ColorAmount));
			particle.Color = ColorMultiply(particle.StartColor, emitter.ColorOverLifetime->Compute(normalizedTime, random));
			
			//particle.Color = emitter.ColorOverLifetime->Compute(normalizedTime, random);
			
			particle.Size = emitter.SizeOverLifetime->Compute(normalizedTime, random);
		}
		
		particles.erase(std::remove_if(std::begin(particles), std::end(particles),
			[](Particle const& p){ return p.TimeToLive <= 0; }), std::end(particles));
	}
}
//-----------------------------------------------------------------------
}// namespace Pomdog
