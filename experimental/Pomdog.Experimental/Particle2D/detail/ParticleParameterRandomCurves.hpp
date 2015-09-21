// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PARTICLEPARAMETERRANDOMCURVES_BD1EDD05_HPP
#define POMDOG_PARTICLEPARAMETERRANDOMCURVES_BD1EDD05_HPP

#include "ParticleParameter.hpp"
#include "ParticleCurveLerp.hpp"
#include "ParticleParameterCurve.hpp"
#include <random>
#include <limits>

namespace Pomdog {
namespace Detail {
namespace Particles {

template <typename T>
class ParticleParameterRandomCurves final: public ParticleParameter<T> {
private:
    ParticleParameterCurve<T> curve1;
    ParticleParameterCurve<T> curve2;

public:
    template <typename Type1, typename Type2>
    explicit ParticleParameterRandomCurves(Type1 && curve1In, Type2 && curve2In)
        : curve1(std::move(curve1In))
        , curve2(std::move(curve2In))
    {
        static_assert(std::is_constructible<decltype(curve1), Type1>::value, "");
        static_assert(std::is_constructible<decltype(curve2), Type2>::value, "");
    }

    T Compute(float normalizedTime, std::mt19937 & random) const override
    {
        return Compute(normalizedTime,
            std::generate_canonical<float, std::numeric_limits<float>::digits>(random));
    }

    T Compute(float normalizedTime, float amount) const override
    {
        return Detail::Particles::ParticleCurveLerp<T>()(
            curve1.Compute(normalizedTime), curve2.Compute(normalizedTime), amount);
    }

    float GenerateVariance(std::mt19937 & random) const override
    {
        return std::generate_canonical<float, std::numeric_limits<float>::digits>(random);
    }
};

} // namespace Particles
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_PARTICLEPARAMETERRANDOMCURVES_BD1EDD05_HPP
