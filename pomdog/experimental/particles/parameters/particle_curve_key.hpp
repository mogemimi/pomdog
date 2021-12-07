// Copyright mogemimi. Distributed under the MIT license.

#pragma once

namespace Pomdog::Detail::Particles {

template <typename T>
struct ParticleCurveKey {
    // std::uint8_t NormalizedTime;
    float TimeSeconds;
    T Value;
};

} // namespace Pomdog::Detail::Particles
