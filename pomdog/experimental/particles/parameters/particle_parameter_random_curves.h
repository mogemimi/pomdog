// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/particles/parameters/particle_curve_lerp.h"
#include "pomdog/experimental/particles/parameters/particle_parameter.h"
#include "pomdog/experimental/particles/parameters/particle_parameter_curve.h"
#include "pomdog/random/xoroshiro128_star_star.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::particles {

template <typename T>
class ParticleParameterRandomCurves final : public ParticleParameter<T> {
private:
    ParticleParameterCurve<T> curve1_;
    ParticleParameterCurve<T> curve2_;

public:
    template <typename Type1, typename Type2>
    explicit ParticleParameterRandomCurves(Type1&& curve1In, Type2&& curve2In)
        : curve1_(std::forward<Type1>(curve1In))
        , curve2_(std::forward<Type2>(curve2In))
    {
        static_assert(std::is_constructible<decltype(curve1_), Type1>::value, "");
        static_assert(std::is_constructible<decltype(curve2_), Type2>::value, "");
    }

    [[nodiscard]] T
    compute(f32 normalizedTime, random::Xoroshiro128StarStar& random) const override
    {
        return compute(normalizedTime,
            std::generate_canonical<f32, std::numeric_limits<f32>::digits>(random));
    }

    [[nodiscard]] T
    compute(f32 normalizedTime, f32 amount) const override
    {
        return detail::particles::ParticleCurveLerp<T>()(
            curve1_.compute(normalizedTime), curve2_.compute(normalizedTime), amount);
    }

    [[nodiscard]] f32
    generateVariance(random::Xoroshiro128StarStar& random) const override
    {
        return std::generate_canonical<f32, std::numeric_limits<f32>::digits>(random);
    }
};

} // namespace pomdog::detail::particles
