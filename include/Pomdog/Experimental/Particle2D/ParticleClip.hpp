// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Experimental/Particle2D/ParticleEmitter.hpp"
#include "Pomdog/Experimental/Particle2D/detail/ParticleEmitterShape.hpp"
#include "Pomdog/Experimental/Particle2D/detail/ParticleParameter.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Radian.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {

class ParticleClip final {
public:
    ParticleEmitter Emitter;

    ///@note StartSpeed (pixel/second)
    std::unique_ptr<Detail::Particles::ParticleParameter<float>> StartSpeed;

    std::unique_ptr<Detail::Particles::ParticleEmitterShape> Shape;

    std::unique_ptr<Detail::Particles::ParticleParameter<Color>> StartColor;

    std::unique_ptr<Detail::Particles::ParticleParameter<Color>> ColorOverLifetime;

    std::unique_ptr<Detail::Particles::ParticleParameter<Radian<float>>> StartRotation;

    std::unique_ptr<Detail::Particles::ParticleParameter<Radian<float>>> RotationOverLifetime;

    std::unique_ptr<Detail::Particles::ParticleParameter<float>> StartSize;

    std::unique_ptr<Detail::Particles::ParticleParameter<float>> SizeOverLifetime;

    Pomdog::Duration Duration = std::chrono::seconds{5};
};

} // namespace Pomdog
