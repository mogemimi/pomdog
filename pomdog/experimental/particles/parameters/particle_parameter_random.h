// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/particles/parameters/particle_curve_lerp.h"
#include "pomdog/experimental/particles/parameters/particle_parameter.h"
#include "pomdog/random/xoroshiro128_star_star.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::particles {

template <typename T>
class ParticleParameterRandom final : public ParticleParameter<T> {
private:
    T min_;
    T max_;

public:
    template <typename Type1, typename Type2>
    explicit ParticleParameterRandom(Type1&& minIn, Type2&& maxIn)
        : min_(std::forward<Type1>(minIn))
        , max_(std::forward<Type2>(maxIn))
    {
        static_assert(std::is_convertible<Type1, T>::value, "");
        static_assert(std::is_convertible<Type2, T>::value, "");
    }

    [[nodiscard]] T
    compute(random::Xoroshiro128StarStar& random) const
    {
        return detail::particles::ParticleCurveLerp<T>()(min_, max_,
            std::generate_canonical<f32, std::numeric_limits<f32>::digits>(random));
    }

    [[nodiscard]] T
    compute(f32, random::Xoroshiro128StarStar& random) const override
    {
        return compute(random);
    }

    [[nodiscard]] T
    compute(f32, f32 amount) const override
    {
        return detail::particles::ParticleCurveLerp<T>()(min_, max_, amount);
    }

    [[nodiscard]] f32
    generateVariance(random::Xoroshiro128StarStar& random) const override
    {
        return std::generate_canonical<f32, std::numeric_limits<f32>::digits>(random);
    }
};

} // namespace pomdog::detail::particles
