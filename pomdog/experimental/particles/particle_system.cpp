// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/particles/particle_system.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/particles/particle_clip.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/quaternion.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector3.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

template <typename RandomGenerator>
Particle createParticle(
    RandomGenerator& random,
    const ParticleClip& clip,
    f32 normalizedTime,
    const Vector3& emitterPosition,
    const Quaternion& emitterRotation)
{
    Particle particle;
    particle.timeToLive = clip.startLifetime;

    {
        // Emitter shape
        POMDOG_ASSERT(clip.shape);
        const auto [emitPosition, emitDirection] = clip.shape->compute(random);

        // Position
        const auto rotation = Matrix4x4::createFromQuaternion(emitterRotation);
        const auto worldMatrix = rotation * Matrix4x4::createTranslation(emitterPosition);
        particle.position = math::transform(emitPosition, worldMatrix);

        // Velocity
        POMDOG_ASSERT(clip.startSpeed);
        const auto startSpeed = clip.startSpeed->compute(normalizedTime, random);
        particle.velocity = math::transform(emitDirection * startSpeed, rotation);
    }
    {
        // Rotation
        POMDOG_ASSERT(clip.startRotation);
        particle.rotation = clip.startRotation->compute(normalizedTime, random);

        POMDOG_ASSERT(clip.rotationOverLifetime);
        particle.rotationVariance = clip.rotationOverLifetime->generateVariance(random);
    }
    {
        // Color
        POMDOG_ASSERT(clip.startColor);
        particle.startColor = clip.startColor->compute(normalizedTime, random);
        particle.color = particle.startColor;

        POMDOG_ASSERT(clip.colorOverLifetime);
        particle.colorVariance = clip.colorOverLifetime->generateVariance(random);
    }
    {
        // Size
        POMDOG_ASSERT(clip.startSize);
        particle.startSize = clip.startSize->compute(normalizedTime, random);
        particle.size = particle.startSize;

        POMDOG_ASSERT(clip.sizeOverLifetime);
        particle.sizeVariance = clip.sizeOverLifetime->generateVariance(random);
    }
    return particle;
}

} // namespace

ParticleSystem::ParticleSystem(const std::shared_ptr<const ParticleClip>& clipIn)
    : clip_(clipIn)
    , elapsedTime_(0)
    , emissionTimer_(0)
    , random_(std::random_device{}())
    , state_(ParticleSystemState::Playing)
{
    POMDOG_ASSERT(clip_);
    POMDOG_ASSERT(clip_->maxParticles > 0);

    if (clip_->maxParticles > 0) {
        particles_.reserve(clip_->maxParticles);
    }
}

void ParticleSystem::simulate(
    const Vector2& emitterPosition,
    const Radian<f32>& emitterRotation,
    const Duration& duration)
{
    simulate(
        Vector3{emitterPosition.x, emitterPosition.y, 0.0f},
        Quaternion::createFromAxisAngle(Vector3::createUnitZ(), emitterRotation.get()),
        duration);
}

void ParticleSystem::simulate(
    const Vector3& emitterPosition,
    const Quaternion& emitterRotation,
    const Duration& duration)
{
    POMDOG_ASSERT(clip_);

    if (state_ != ParticleSystemState::Playing) {
        return;
    }

    elapsedTime_ += duration;

    if (clip_->looping && elapsedTime_ > clip_->duration) {
        elapsedTime_ = Duration::zero();
    }

    if (clip_->looping || elapsedTime_ <= clip_->duration) {
        emissionTimer_ += duration;

        POMDOG_ASSERT(clip_->emissionRate > 0);
        POMDOG_ASSERT(clip_->emissionRateOverTime > 0);
        POMDOG_ASSERT(clip_->duration > Duration::zero());
        const auto x0 = 1.0 / clip_->emissionRate;
        const auto x1 = 1.0 / clip_->emissionRateOverTime;
        const auto normalizedTime = elapsedTime_.count() / clip_->duration.count();
        const auto lerpInterval = math::lerp(x0, x1, normalizedTime);
        const auto emissionInterval = std::max(std::numeric_limits<Duration>::epsilon(), Duration{lerpInterval});

        POMDOG_ASSERT(emissionInterval.count() > 0);
        POMDOG_ASSERT(clip_->duration.count() > 0);
        const auto normalizedTimeAsFloat = static_cast<f32>(normalizedTime);

        const auto maxParticles = static_cast<std::size_t>(clip_->maxParticles);
        while ((particles_.size() < maxParticles) && (emissionTimer_ >= emissionInterval)) {
            emissionTimer_ -= emissionInterval;

            auto particle = createParticle(
                random_,
                *clip_,
                normalizedTimeAsFloat,
                emitterPosition,
                emitterRotation);

            particles_.push_back(std::move(particle));
        }
    }
    {
        POMDOG_ASSERT(clip_->startLifetime > 0.0f);
        const auto invertedLifetime = 1.0f / clip_->startLifetime;

        for (auto& particle : particles_) {
            auto oldTimeToLive = particle.timeToLive;
            particle.timeToLive -= static_cast<f32>(duration.count());
            if (particle.timeToLive <= 0.0f) {
                continue;
            }

            const auto deltaTime = oldTimeToLive - particle.timeToLive;
            const auto particleAcceleration = Vector3{1.0f, 1.0f, 0.0f};
            const auto gravityAcceleration = Vector3{0.0f, -clip_->gravityModifier, 0.0f};

            particle.velocity += (particleAcceleration * gravityAcceleration * deltaTime);
            particle.position += (particle.velocity * deltaTime);

            const auto normalizedTime = 1.0f - particle.timeToLive * invertedLifetime;

            POMDOG_ASSERT(clip_->colorOverLifetime);
            particle.color = math::multiply(particle.startColor,
                clip_->colorOverLifetime->compute(normalizedTime, particle.colorVariance));

            // Rotation
            POMDOG_ASSERT(clip_->rotationOverLifetime);
            particle.rotation =
                particle.rotation +
                deltaTime * clip_->rotationOverLifetime->compute(normalizedTime, particle.rotationVariance);

            // Size
            POMDOG_ASSERT(clip_->sizeOverLifetime);
            particle.size = particle.startSize * clip_->sizeOverLifetime->compute(normalizedTime, particle.sizeVariance);
        }

        particles_.erase(
            std::remove_if(
                std::begin(particles_),
                std::end(particles_),
                [](const Particle& p) { return p.timeToLive <= 0; }),
            std::end(particles_));
    }
}

void ParticleSystem::play()
{
    state_ = ParticleSystemState::Playing;
}

void ParticleSystem::pause()
{
    state_ = ParticleSystemState::Paused;
}

void ParticleSystem::stop()
{
    state_ = ParticleSystemState::Stopped;
    elapsedTime_ = Duration::zero();
    particles_.clear();
}

bool ParticleSystem::isAlive() const
{
    POMDOG_ASSERT(clip_);
    return clip_->looping || (elapsedTime_ < clip_->duration) || !particles_.empty();
}

bool ParticleSystem::isLoop() const noexcept
{
    POMDOG_ASSERT(clip_);
    return clip_->looping;
}

} // namespace pomdog
