//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "ParticleSystem.hpp"
#include "ParticleClip.hpp"

namespace Pomdog {
namespace {
//-----------------------------------------------------------------------
template <typename RandomGenerator>
static Particle CreateParticle(RandomGenerator & random, ParticleClip const& clip,
	ParticleEmitter const& emitter, float normalizedTime, Transform2D const& transform)
{
	Particle particle;
	{
		particle.TimeToLive = emitter.StartLifetime;
	}
	{
		// emitter shape
		Vector2 emitPosition = Vector2::Zero;
		Radian<float> emitAngle = 0.0f;

		POMDOG_ASSERT(clip.Shape);
		clip.Shape->Compute(random, emitPosition, emitAngle);
		
		// Position
		auto worldMatrix = Matrix4x4::CreateRotationZ(transform.Rotation) * Matrix4x4::CreateTranslation({transform.Position, 0});
		particle.Position = Vector2::Transform(emitPosition, worldMatrix);
		
		// Velocity
		POMDOG_ASSERT(clip.StartSpeed);
		auto radius = clip.StartSpeed->Compute(normalizedTime, random);
		auto angle = (emitAngle + transform.Rotation).value;
		
		particle.Velocity.X = std::cos(angle) * radius;
		particle.Velocity.Y = std::sin(angle) * radius;
	}
	{
		// Rotation
		POMDOG_ASSERT(clip.StartRotation);
		particle.Rotation = clip.StartRotation->Compute(normalizedTime, random);

		POMDOG_ASSERT(clip.RotationOverLifetime);
		particle.RotationVariance = clip.RotationOverLifetime->GenerateVariance(random);
	}
	{
		// Color
		POMDOG_ASSERT(clip.StartColor);
		particle.StartColor = clip.StartColor->Compute(normalizedTime, random);
		particle.Color = particle.StartColor;
		
		POMDOG_ASSERT(clip.ColorOverLifetime);
		particle.ColorVariance = clip.ColorOverLifetime->GenerateVariance(random);
	}
	{
		// Size
		POMDOG_ASSERT(clip.StartSize);
		particle.StartSize = clip.StartSize->Compute(normalizedTime, random);
		particle.Size = particle.StartSize;
		
		POMDOG_ASSERT(clip.SizeOverLifetime);
		particle.SizeVariance = clip.SizeOverLifetime->GenerateVariance(random);
	}
	return std::move(particle);
}
//-----------------------------------------------------------------------
}// unnamed namespace
//-----------------------------------------------------------------------
ParticleSystem::ParticleSystem(std::shared_ptr<ParticleClip const> const& clipIn)
	: clip(clipIn)
	, erapsedTime(0)
	, emissionTimer(0)
	, random(std::random_device{}())
	, state(ParticleSystemState::Playing)
	, enableEmission(true)
{
	POMDOG_ASSERT(clip);
	emitter = clip->Emitter;
	particles.reserve(emitter.MaxParticles);
}
//-----------------------------------------------------------------------
void ParticleSystem::Simulate(GameObject & gameObject, DurationSeconds const& duration)
{
	if (state != ParticleSystemState::Playing) {
		return;
	}

	Transform2D emitterTransform;
	if (auto transform = gameObject.Component<Transform2D>()) {
		emitterTransform = *transform;
	}

	erapsedTime += duration;
	
	if (emitter.Looping && erapsedTime > clip->Duration)
	{
		erapsedTime = DurationSeconds{0};
	}

	if (emitter.Looping || erapsedTime <= clip->Duration)
	{
		emissionTimer += duration;
		
		POMDOG_ASSERT(emitter.EmissionRate > 0);
		auto emissionInterval = std::max(std::numeric_limits<DurationSeconds>::epsilon(),
			DurationSeconds(1) / emitter.EmissionRate);
		
		POMDOG_ASSERT(emissionInterval.count() > 0);

		POMDOG_ASSERT(clip->Duration.count() > 0);
		float normalizedTime = erapsedTime / clip->Duration;
		
		while ((particles.size() < emitter.MaxParticles) && (emissionTimer >= emissionInterval))
		{
			emissionTimer -= emissionInterval;

			auto particle = CreateParticle(random, *clip, emitter, normalizedTime, emitterTransform);
			particles.push_back(std::move(particle));
		}
	}
	{
		for (auto & particle: particles)
		{
			auto oldTimeToLive = particle.TimeToLive;
			particle.TimeToLive -= duration.count();
			if (particle.TimeToLive <= 0.0f) {
				continue;
			}
			
			auto deltaTime = oldTimeToLive - particle.TimeToLive;
			
			Vector2 particleAcceleration {1, 1};
			Vector2 gravityAcceleration {0, -emitter.GravityModifier};
			particle.Velocity += (particleAcceleration * gravityAcceleration * deltaTime);
			particle.Position += (particle.Velocity * deltaTime);
			
			float normalizedTime = 1.0f - particle.TimeToLive / emitter.StartLifetime;
			
			// Color
			auto MultiplyColors = [](Color const& a, Color const& b)->Color {
				return {
					static_cast<uint8_t>(MathHelper::Clamp((a.R()/255.0f) * b.R(), 0.0f, 255.0f)),
					static_cast<uint8_t>(MathHelper::Clamp((a.G()/255.0f) * b.G(), 0.0f, 255.0f)),
					static_cast<uint8_t>(MathHelper::Clamp((a.B()/255.0f) * b.B(), 0.0f, 255.0f)),
					static_cast<uint8_t>(MathHelper::Clamp((a.A()/255.0f) * b.A(), 0.0f, 255.0f))};
			};
			
			POMDOG_ASSERT(clip->ColorOverLifetime);
			particle.Color = MultiplyColors(particle.StartColor,
				clip->ColorOverLifetime->Compute(normalizedTime, particle.ColorVariance));
			
			// Rotation
			POMDOG_ASSERT(clip->RotationOverLifetime);
			particle.Rotation = particle.Rotation
				+ deltaTime * clip->RotationOverLifetime->Compute(normalizedTime, particle.RotationVariance);
			
			// Size
			POMDOG_ASSERT(clip->SizeOverLifetime);
			particle.Size = particle.StartSize * clip->SizeOverLifetime->Compute(normalizedTime, particle.SizeVariance);
		}
		
		particles.erase(std::remove_if(std::begin(particles), std::end(particles),
			[](Particle const& p){ return p.TimeToLive <= 0; }), std::end(particles));
	}
}
//-----------------------------------------------------------------------
void ParticleSystem::Play()
{
	state = ParticleSystemState::Playing;
}
//-----------------------------------------------------------------------
void ParticleSystem::Pause()
{
	state = ParticleSystemState::Paused;
}
//-----------------------------------------------------------------------
void ParticleSystem::Stop()
{
	state = ParticleSystemState::Stopped;
	erapsedTime = DurationSeconds::zero();
	particles.clear();
}
//-----------------------------------------------------------------------
bool ParticleSystem::IsAlive() const
{
	return emitter.Looping
		|| (erapsedTime < clip->Duration)
		|| !particles.empty();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
