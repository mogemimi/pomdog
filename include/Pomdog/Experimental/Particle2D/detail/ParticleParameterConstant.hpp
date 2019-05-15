// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "ParticleParameter.hpp"
#include <utility>

namespace Pomdog {
namespace Detail {
namespace Particles {

template <typename T>
class ParticleParameterConstant final : public ParticleParameter<T> {
private:
    T value;

public:
    template <typename InType>
    explicit ParticleParameterConstant(InType&& valueIn)
        : value(std::move(valueIn))
    {
        static_assert(std::is_convertible<InType, T>::value, "");
    }

    T Compute(float, std::mt19937&) const override
    {
        return value;
    }

    T Compute(float, float) const override
    {
        return value;
    }

    float GenerateVariance(std::mt19937&) const override
    {
        return 1.0f;
    }
};

} // namespace Particles
} // namespace Detail
} // namespace Pomdog
