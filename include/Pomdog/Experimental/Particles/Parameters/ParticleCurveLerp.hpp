// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include "Pomdog/Math/Radian.hpp"
#include "Pomdog/Math/Vector3.hpp"

namespace Pomdog::Detail::Particles {

template <typename T>
struct ParticleCurveLerp {
    T operator()(const T& a, const T& b, float amount)
    {
        return MathHelper::Lerp(a, b, amount);
    }
};

template <>
struct ParticleCurveLerp<Radian<float>> {
    Radian<float> operator()(const Radian<float>& a, const Radian<float>& b, float amount)
    {
        return MathHelper::Lerp(a.value, b.value, amount);
    }
};

template <>
struct ParticleCurveLerp<Color> {
    Color operator()(const Color& a, const Color& b, float amount)
    {
        return Color::Lerp(a, b, amount);
    }
};

template <>
struct ParticleCurveLerp<Vector3> {
    Vector3 operator()(const Vector3& a, const Vector3& b, float amount)
    {
        return Vector3::Lerp(a, b, amount);
    }
};

} // namespace Pomdog::Detail::Particles
