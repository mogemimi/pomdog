// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/particles/parameters/particle_parameter.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::particles {

template <typename T>
class ParticleParameterConstant final : public ParticleParameter<T> {
private:
    T value_;

public:
    template <typename InType>
    explicit ParticleParameterConstant(InType&& valueIn)
        : value_(std::forward<InType>(valueIn))
    {
        static_assert(std::is_convertible<InType, T>::value, "");
    }

    [[nodiscard]] T
    compute(f32, random::Xoroshiro128StarStar&) const override
    {
        return value_;
    }

    [[nodiscard]] T
    compute(f32, f32) const override
    {
        return value_;
    }

    [[nodiscard]] f32
    generateVariance(random::Xoroshiro128StarStar&) const override
    {
        return 1.0f;
    }
};

} // namespace pomdog::detail::particles
