// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_BEAMSYSTEM_616FE7EE_HPP
#define POMDOG_BEAMSYSTEM_616FE7EE_HPP

#include "Beam.hpp"
#include "BeamBranching.hpp"
#include "BeamEmitter.hpp"
#include "Pomdog.Experimental/Gameplay2D/Transform2D.hpp"
#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include <cstdint>
#include <vector>
#include <random>

namespace Pomdog {

class BeamSystem {
public:
    BeamSystem();

    void Update(Duration const& frameDuration, Transform2D const& emitterTransform, Vector2 const& target);

    static Vector2 CreateTarget(Transform2D const& emitterTransform, float distance);

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

#endif // POMDOG_BEAMSYSTEM_616FE7EE_HPP
