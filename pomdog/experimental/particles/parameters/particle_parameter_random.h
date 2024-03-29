// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/particles/parameters/particle_curve_lerp.h"
#include "pomdog/experimental/particles/parameters/particle_parameter.h"
#include "pomdog/experimental/random/xoroshiro128_star_star.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::particles {

template <typename T>
class ParticleParameterRandom final : public ParticleParameter<T> {
private:
    T min;
    T max;

public:
    template <typename Type1, typename Type2>
    explicit ParticleParameterRandom(Type1&& minIn, Type2&& maxIn)
        : min(std::forward<Type1>(minIn))
        , max(std::forward<Type2>(maxIn))
    {
        static_assert(std::is_convertible<Type1, T>::value, "");
        static_assert(std::is_convertible<Type2, T>::value, "");
    }

    T Compute(random::Xoroshiro128StarStar& random) const
    {
        return detail::particles::ParticleCurveLerp<T>()(min, max,
            std::generate_canonical<float, std::numeric_limits<float>::digits>(random));
    }

    T Compute(float, random::Xoroshiro128StarStar& random) const override
    {
        return Compute(random);
    }

    T Compute(float, float amount) const override
    {
        return detail::particles::ParticleCurveLerp<T>()(min, max, amount);
    }

    float GenerateVariance(random::Xoroshiro128StarStar& random) const override
    {
        return std::generate_canonical<float, std::numeric_limits<float>::digits>(random);
    }
};

} // namespace pomdog::detail::particles
