// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Beam.hpp"
#include "BeamBranching.hpp"
#include "BeamEmitter.hpp"
#include "Pomdog.Experimental/Gameplay2D/Transform.hpp"
#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include <cstdint>
#include <vector>
#include <random>

namespace Pomdog {

class BeamSystem {
public:
    BeamSystem();

    void Update(Duration const& frameDuration, Transform const& emitterTransform, Vector2 const& target);

    static Vector2 CreateTarget(Transform const& emitterTransform, float distance);

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
