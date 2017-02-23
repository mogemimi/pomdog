// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

namespace Pomdog {
namespace Detail {
namespace Particles {

template <typename T>
struct ParticleCurveKey {
    //std::uint8_t NormalizedTime;
    float TimeSeconds;
    T Value;
};

} // namespace Particles
} // namespace Detail
} // namespace Pomdog
