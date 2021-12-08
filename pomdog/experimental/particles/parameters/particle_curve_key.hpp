// Copyright mogemimi. Distributed under the MIT license.

#pragma once

namespace pomdog::detail::particles {

template <typename T>
struct ParticleCurveKey {
    // std::uint8_t NormalizedTime;
    float TimeSeconds;
    T Value;
};

} // namespace pomdog::detail::particles
