// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/math/color.h"
#include "pomdog/math/math.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/vector3.h"

namespace pomdog::detail::particles {

template <typename T>
struct ParticleCurveLerp final {
    T operator()(const T& a, const T& b, float amount)
    {
        return math::lerp(a, b, amount);
    }
};

template <>
struct ParticleCurveLerp<Radian<float>> final {
    Radian<float> operator()(const Radian<float>& a, const Radian<float>& b, float amount)
    {
        return math::lerp(a.value, b.value, amount);
    }
};

template <>
struct ParticleCurveLerp<Color> final {
    Color operator()(const Color& a, const Color& b, float amount)
    {
        return math::lerp(a, b, amount);
    }
};

template <>
struct ParticleCurveLerp<Vector3> final {
    Vector3 operator()(const Vector3& a, const Vector3& b, float amount)
    {
        return math::lerp(a, b, amount);
    }
};

} // namespace pomdog::detail::particles
