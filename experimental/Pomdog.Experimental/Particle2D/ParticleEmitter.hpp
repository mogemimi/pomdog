// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PARTICLEEMITTER_69E1B461_HPP
#define POMDOG_PARTICLEEMITTER_69E1B461_HPP

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

#endif // POMDOG_PARTICLEEMITTER_69E1B461_HPP
