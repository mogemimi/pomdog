// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PARTICLECURVELERP_7AA431A6_08A8_4716_AA4F_0F52D3AC7793_HPP
#define POMDOG_PARTICLECURVELERP_7AA431A6_08A8_4716_AA4F_0F52D3AC7793_HPP

#if _MSC_VER > 1000
#pragma once
#endif

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

}// namespace Particles
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLECURVELERP_7AA431A6_08A8_4716_AA4F_0F52D3AC7793_HPP)
