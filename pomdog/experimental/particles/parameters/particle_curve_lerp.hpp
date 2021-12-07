// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/math/color.hpp"
#include "pomdog/math/math.hpp"
#include "pomdog/math/radian.hpp"
#include "pomdog/math/vector3.hpp"

namespace Pomdog::Detail::Particles {

template <typename T>
struct ParticleCurveLerp final {
    T operator()(const T& a, const T& b, float amount)
    {
        return Math::Lerp(a, b, amount);
    }
};

template <>
struct ParticleCurveLerp<Radian<float>> final {
    Radian<float> operator()(const Radian<float>& a, const Radian<float>& b, float amount)
    {
        return Math::Lerp(a.value, b.value, amount);
    }
};

template <>
struct ParticleCurveLerp<Color> final {
    Color operator()(const Color& a, const Color& b, float amount)
    {
        return Color::Lerp(a, b, amount);
    }
};

template <>
struct ParticleCurveLerp<Vector3> final {
    Vector3 operator()(const Vector3& a, const Vector3& b, float amount)
    {
        return Vector3::Lerp(a, b, amount);
    }
};

} // namespace Pomdog::Detail::Particles
