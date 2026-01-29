// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/math/color.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/vector3.h"

namespace pomdog::detail::particles {

template <typename T>
struct ParticleCurveLerp final {
    T operator()(const T& a, const T& b, f32 amount)
    {
        return math::lerp(a, b, amount);
    }
};

template <>
struct ParticleCurveLerp<Radian<f32>> final {
    Radian<f32> operator()(const Radian<f32>& a, const Radian<f32>& b, f32 amount)
    {
        return math::lerp(a.get(), b.get(), amount);
    }
};

template <>
struct ParticleCurveLerp<Color> final {
    Color operator()(const Color& a, const Color& b, f32 amount)
    {
        return math::lerp(a, b, amount);
    }
};

template <>
struct ParticleCurveLerp<Vector3> final {
    Vector3 operator()(const Vector3& a, const Vector3& b, f32 amount)
    {
        return math::lerp(a, b, amount);
    }
};

} // namespace pomdog::detail::particles
