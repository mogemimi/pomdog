// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/particles/parameters/particle_parameter.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::particles {

template <typename T>
class ParticleParameterConstant final : public ParticleParameter<T> {
private:
    T value;

public:
    template <typename InType>
    explicit ParticleParameterConstant(InType&& valueIn)
        : value(std::forward<InType>(valueIn))
    {
        static_assert(std::is_convertible<InType, T>::value, "");
    }

    T Compute(float, random::Xoroshiro128StarStar&) const override
    {
        return value;
    }

    T Compute(float, float) const override
    {
        return value;
    }

    float GenerateVariance(random::Xoroshiro128StarStar&) const override
    {
        return 1.0f;
    }
};

} // namespace pomdog::detail::particles
