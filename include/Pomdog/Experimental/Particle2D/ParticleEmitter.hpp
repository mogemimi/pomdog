// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

class ParticleEmitter final {
public:
    float StartDelay = 0.0f;

    float StartLifetime = 1.0f;

    float GravityModifier = 0.0f;

    std::uint16_t MaxParticles = 128;

    std::uint16_t EmissionRate = 16;

    // float InheritVelocity;

    // ParticleSimulationSpace SimulationSpace;

    bool Looping = true;

    // bool Prewarm = false;
};

} // namespace Pomdog
