// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/particles/parameters/particle_curve_lerp.hpp"
#include "pomdog/experimental/particles/parameters/particle_parameter.hpp"
#include "pomdog/experimental/particles/parameters/particle_parameter_curve.hpp"
#include "pomdog/experimental/random/xoroshiro128_star_star.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::Particles {

template <typename T>
class ParticleParameterRandomCurves final : public ParticleParameter<T> {
private:
    ParticleParameterCurve<T> curve1;
    ParticleParameterCurve<T> curve2;

public:
    template <typename Type1, typename Type2>
    explicit ParticleParameterRandomCurves(Type1&& curve1In, Type2&& curve2In)
        : curve1(std::move(curve1In))
        , curve2(std::move(curve2In))
    {
        static_assert(std::is_constructible<decltype(curve1), Type1>::value, "");
        static_assert(std::is_constructible<decltype(curve2), Type2>::value, "");
    }

    T Compute(float normalizedTime, Random::Xoroshiro128StarStar& random) const override
    {
        return Compute(normalizedTime,
            std::generate_canonical<float, std::numeric_limits<float>::digits>(random));
    }

    T Compute(float normalizedTime, float amount) const override
    {
        return Detail::Particles::ParticleCurveLerp<T>()(
            curve1.Compute(normalizedTime), curve2.Compute(normalizedTime), amount);
    }

    float GenerateVariance(Random::Xoroshiro128StarStar& random) const override
    {
        return std::generate_canonical<float, std::numeric_limits<float>::digits>(random);
    }
};

} // namespace Pomdog::Detail::Particles
