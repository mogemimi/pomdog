// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

class ParticleEmitter {
public:
    float StartDelay = 0;

    float StartLifetime = 1;

    float GravityModifier = 0;

    std::uint16_t MaxParticles = 128;

    std::uint16_t EmissionRate = 16;

    //float InheritVelocity;

    //ParticleSimulationSpace SimulationSpace;

    bool Looping = true;

    //bool Prewarm = false;
};

} // namespace Pomdog
