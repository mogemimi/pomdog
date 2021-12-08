// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/particles/particle_system.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/particles/particle_clip.hpp"
#include "pomdog/math/math.hpp"
#include "pomdog/math/matrix4x4.hpp"
#include "pomdog/math/quaternion.hpp"
#include "pomdog/math/radian.hpp"
#include "pomdog/math/vector2.hpp"
#include "pomdog/math/vector3.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

template <typename RandomGenerator>
Particle CreateParticle(
    RandomGenerator& random,
    const ParticleClip& clip,
    float normalizedTime,
    const Vector3& emitterPosition,
    const Quaternion& emitterRotation)
{
    Particle particle;
    particle.TimeToLive = clip.StartLifetime;

    {
        // Emitter shape
        POMDOG_ASSERT(clip.Shape);
        const auto [emitPosition, emitDirection] = clip.Shape->Compute(random);

        // Position
        const auto rotation = Matrix4x4::CreateFromQuaternion(emitterRotation);
        const auto worldMatrix = rotation * Matrix4x4::CreateTranslation(emitterPosition);
        particle.Position = Vector3::Transform(emitPosition, worldMatrix);

        // Velocity
        POMDOG_ASSERT(clip.StartSpeed);
        const auto startSpeed = clip.StartSpeed->Compute(normalizedTime, random);
        particle.Velocity = Vector3::Transform(emitDirection * startSpeed, rotation);
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

} // namespace

ParticleSystem::ParticleSystem(const std::shared_ptr<ParticleClip const>& clipIn)
    : clip(clipIn)
    , erapsedTime(0)
    , emissionTimer(0)
    , random(std::random_device{}())
    , state(ParticleSystemState::Playing)
{
    POMDOG_ASSERT(clip);
    POMDOG_ASSERT(clip->MaxParticles > 0);

    if (clip->MaxParticles > 0) {
        particles.reserve(clip->MaxParticles);
    }
}

void ParticleSystem::Simulate(
    const Vector2& emitterPosition,
    const Radian<float>& emitterRotation,
    const Duration& duration)
{
    Simulate(
        Vector3{emitterPosition.X, emitterPosition.Y, 0.0f},
        Quaternion::CreateFromAxisAngle(Vector3::UnitZ, emitterRotation.value),
        duration);
}

void ParticleSystem::Simulate(
    const Vector3& emitterPosition,
    const Quaternion& emitterRotation,
    const Duration& duration)
{
    POMDOG_ASSERT(clip);

    if (state != ParticleSystemState::Playing) {
        return;
    }

    erapsedTime += duration;

    if (clip->Looping && erapsedTime > clip->Duration) {
        erapsedTime = Duration::zero();
    }

    if (clip->Looping || erapsedTime <= clip->Duration) {
        emissionTimer += duration;

        POMDOG_ASSERT(clip->EmissionRate > 0);
        POMDOG_ASSERT(clip->EmissionRateOverTime > 0);
        POMDOG_ASSERT(clip->Duration > Duration::zero());
        const auto x0 = 1.0 / clip->EmissionRate;
        const auto x1 = 1.0 / clip->EmissionRateOverTime;
        const auto normalizedTime = erapsedTime.count() / clip->Duration.count();
        const auto lerpInterval = math::Lerp(x0, x1, normalizedTime);
        const auto emissionInterval = std::max(std::numeric_limits<Duration>::epsilon(), Duration{lerpInterval});

        POMDOG_ASSERT(emissionInterval.count() > 0);
        POMDOG_ASSERT(clip->Duration.count() > 0);
        const auto normalizedTimeAsFloat = static_cast<float>(normalizedTime);

        const auto maxParticles = static_cast<std::size_t>(clip->MaxParticles);
        while ((particles.size() < maxParticles) && (emissionTimer >= emissionInterval)) {
            emissionTimer -= emissionInterval;

            auto particle = CreateParticle(
                random,
                *clip,
                normalizedTimeAsFloat,
                emitterPosition,
                emitterRotation);

            particles.push_back(std::move(particle));
        }
    }
    {
        POMDOG_ASSERT(clip->StartLifetime > 0.0f);
        const auto invertedLifetime = 1.0f / clip->StartLifetime;

        for (auto& particle : particles) {
            auto oldTimeToLive = particle.TimeToLive;
            particle.TimeToLive -= static_cast<float>(duration.count());
            if (particle.TimeToLive <= 0.0f) {
                continue;
            }

            const auto deltaTime = oldTimeToLive - particle.TimeToLive;
            const auto particleAcceleration = Vector3{1.0f, 1.0f, 0.0f};
            const auto gravityAcceleration = Vector3{0.0f, -clip->GravityModifier, 0.0f};

            particle.Velocity += (particleAcceleration * gravityAcceleration * deltaTime);
            particle.Position += (particle.Velocity * deltaTime);

            const auto normalizedTime = 1.0f - particle.TimeToLive * invertedLifetime;

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
    POMDOG_ASSERT(clip);
    return clip->Looping || (erapsedTime < clip->Duration) || !particles.empty();
}

bool ParticleSystem::IsLoop() const noexcept
{
    POMDOG_ASSERT(clip);
    return clip->Looping;
}

} // namespace pomdog
