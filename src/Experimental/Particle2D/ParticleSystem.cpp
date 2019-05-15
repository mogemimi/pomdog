// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Particle2D/ParticleSystem.hpp"
#include "Pomdog/Experimental/Particle2D/ParticleClip.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>

namespace Pomdog {
namespace {

template <typename RandomGenerator>
Particle CreateParticle(
    RandomGenerator& random,
    const ParticleClip& clip,
    const ParticleEmitter& emitter,
    float normalizedTime,
    const Vector2& emitterPosition,
    const Radian<float>& emitterRotation)
{
    Particle particle;
    {
        particle.TimeToLive = emitter.StartLifetime;
    }
    {
        // emitter shape
        auto emitPosition = Vector3::Zero;
        Radian<float> emitAngle = 0.0f;

        POMDOG_ASSERT(clip.Shape);
        clip.Shape->Compute(random, emitPosition, emitAngle);

        // Position
        const auto worldMatrix = Matrix4x4::CreateRotationZ(emitterRotation) * Matrix4x4::CreateTranslation({emitterPosition, 0});
        particle.Position = Vector3::Transform(emitPosition, worldMatrix);

        // Velocity
        POMDOG_ASSERT(clip.StartSpeed);
        const auto radius = clip.StartSpeed->Compute(normalizedTime, random);
        const auto angle = (emitAngle + emitterRotation).value;

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
    return particle;
}

} // unnamed namespace

ParticleSystem::ParticleSystem(const std::shared_ptr<ParticleClip const>& clipIn)
    : clip(clipIn)
    , erapsedTime(0)
    , emissionTimer(0)
    , random(std::random_device{}())
    , state(ParticleSystemState::Playing)
{
    POMDOG_ASSERT(clip);
    emitter = clip->Emitter;
    particles.reserve(emitter.MaxParticles);
}

void ParticleSystem::Simulate(
    const Vector2& emitterPosition,
    const Radian<float>& emitterRotation,
    const Duration& duration)
{
    if (state != ParticleSystemState::Playing) {
        return;
    }

    erapsedTime += duration;

    if (emitter.Looping && erapsedTime > clip->Duration) {
        erapsedTime = Duration::zero();
    }

    if (emitter.Looping || erapsedTime <= clip->Duration) {
        emissionTimer += duration;

        POMDOG_ASSERT(emitter.EmissionRate > 0);
        auto emissionInterval = std::max(std::numeric_limits<Duration>::epsilon(),
            Duration(1) / emitter.EmissionRate);

        POMDOG_ASSERT(emissionInterval.count() > 0);

        POMDOG_ASSERT(clip->Duration.count() > 0);
        const auto normalizedTime = static_cast<float>(erapsedTime / clip->Duration);

        while ((particles.size() < emitter.MaxParticles) && (emissionTimer >= emissionInterval)) {
            emissionTimer -= emissionInterval;

            auto particle = CreateParticle(
                random, *clip, emitter, normalizedTime, emitterPosition, emitterRotation);

            particles.push_back(std::move(particle));
        }
    }
    {
        for (auto& particle : particles) {
            auto oldTimeToLive = particle.TimeToLive;
            particle.TimeToLive -= static_cast<float>(duration.count());
            if (particle.TimeToLive <= 0.0f) {
                continue;
            }

            const auto deltaTime = oldTimeToLive - particle.TimeToLive;
            const auto particleAcceleration = Vector3{1.0f, 1.0f, 0.0f};
            const auto gravityAcceleration = Vector3{0.0f, -emitter.GravityModifier, 0.0f};

            particle.Velocity += (particleAcceleration * gravityAcceleration * deltaTime);
            particle.Position += (particle.Velocity * deltaTime);

            float normalizedTime = 1.0f - particle.TimeToLive / emitter.StartLifetime;

            POMDOG_ASSERT(clip->ColorOverLifetime);
            particle.Color = Color::Multiply(particle.StartColor,
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
            [](const Particle& p){ return p.TimeToLive <= 0; }), std::end(particles));
    }
}

void ParticleSystem::Play()
{
    state = ParticleSystemState::Playing;
}

void ParticleSystem::Pause()
{
    state = ParticleSystemState::Paused;
}

void ParticleSystem::Stop()
{
    state = ParticleSystemState::Stopped;
    erapsedTime = Duration::zero();
    particles.clear();
}

bool ParticleSystem::IsAlive() const
{
    return emitter.Looping
        || (erapsedTime < clip->Duration)
        || !particles.empty();
}

} // namespace Pomdog
