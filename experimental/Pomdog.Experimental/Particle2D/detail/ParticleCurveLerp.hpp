// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/MathHelper.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Radian.hpp"

namespace Pomdog {
namespace Detail {
namespace Particles {

template <typename T>
struct ParticleCurveLerp {
    T operator()(T const& a, T const& b, float amount)
    {
        return MathHelper::Lerp(a, b, amount);
    }
};

template <>
struct ParticleCurveLerp<Radian<float>> {
    Radian<float> operator()(Radian<float> const& a, Radian<float> const& b, float amount)
    {
        return MathHelper::Lerp(a.value, b.value, amount);
    }
};

template <>
struct ParticleCurveLerp<Color> {
    Color operator()(Color const& a, Color const& b, float amount)
    {
        return Color::Lerp(a, b, amount);
    }
};

} // namespace Particles
} // namespace Detail
} // namespace Pomdog
