// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Particles/Parameters/ParticleCurveLerp.hpp"
#include "Pomdog/Experimental/Particles/Parameters/ParticleParameter.hpp"
#include "Pomdog/Experimental/Random/Xoroshiro128StarStar.hpp"
#include <limits>
#include <random>

namespace Pomdog::Detail::Particles {

template <typename T>
class ParticleParameterRandom final : public ParticleParameter<T> {
private:
    T min;
    T max;

public:
    template <typename Type1, typename Type2>
    explicit ParticleParameterRandom(Type1&& minIn, Type2&& maxIn)
        : min(std::move(minIn))
        , max(std::move(maxIn))
    {
        static_assert(std::is_convertible<Type1, T>::value, "");
        static_assert(std::is_convertible<Type2, T>::value, "");
    }

    T Compute(Random::Xoroshiro128StarStar& random) const
    {
        return Detail::Particles::ParticleCurveLerp<T>()(min, max,
            std::generate_canonical<float, std::numeric_limits<float>::digits>(random));
    }

    T Compute(float, Random::Xoroshiro128StarStar& random) const override
    {
        return Compute(random);
    }

    T Compute(float, float amount) const override
    {
        return Detail::Particles::ParticleCurveLerp<T>()(min, max, amount);
    }

    float GenerateVariance(Random::Xoroshiro128StarStar& random) const override
    {
        return std::generate_canonical<float, std::numeric_limits<float>::digits>(random);
    }
};

} // namespace Pomdog::Detail::Particles
