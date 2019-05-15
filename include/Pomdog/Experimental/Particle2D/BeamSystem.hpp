// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Particle2D/Beam.hpp"
#include "Pomdog/Experimental/Particle2D/BeamBranching.hpp"
#include "Pomdog/Experimental/Particle2D/BeamEmitter.hpp"
#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include <cstdint>
#include <random>
#include <vector>

namespace Pomdog {

class BeamSystem final {
public:
    BeamSystem();

    void Update(const Duration& frameDuration, const Vector2& emitterPosition, const Vector2& target);

    static Vector2 CreateTarget(
        const Vector2& emitterPosition,
        const Radian<float>& emitterRotation,
        float distance);

public:
    BeamEmitter emitter;
    BeamBranching branching;

    std::vector<Beam> beams;

private:
    Duration erapsedTime;
    Duration emissionTimer;
    std::mt19937 random;
};

} // namespace Pomdog
