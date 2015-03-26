// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PARTICLECURVEKEY_6EBF4743_HPP
#define POMDOG_PARTICLECURVEKEY_6EBF4743_HPP

namespace Pomdog {
namespace Detail {
namespace Particles {

template <typename T>
struct ParticleCurveKey {
    //std::uint8_t NormalizedTime;
    float TimeSeconds;
    T Value;
};

}// namespace Particles
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_PARTICLECURVEKEY_6EBF4743_HPP
