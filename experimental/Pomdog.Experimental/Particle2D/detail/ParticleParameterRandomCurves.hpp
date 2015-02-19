//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_PARTICLEPARAMETERRANDOMCURVES_BD1EDD05_8FD4_44FB_A7F3_720E3833A351_HPP
#define POMDOG_PARTICLEPARAMETERRANDOMCURVES_BD1EDD05_8FD4_44FB_A7F3_720E3833A351_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "ParticleParameter.hpp"
#include "ParticleCurveLerp.hpp"
#include "ParticleParameterCurve.hpp"
#include <random>
#include <limits>

namespace Pomdog {
namespace Details {
namespace Particles {

template <typename T>
class ParticleParameterRandomCurves final: public ParticleParameter<T> {
private:
	ParticleParameterCurve<T> curve1;
	ParticleParameterCurve<T> curve2;

public:
	template <typename Type1, typename Type2>
	explicit ParticleParameterRandomCurves(Type1 && curve1In, Type2 && curve2In)
		: curve1(std::move(curve1In))
		, curve2(std::move(curve2In))
	{
		static_assert(std::is_constructible<decltype(curve1), Type1>::value, "");
		static_assert(std::is_constructible<decltype(curve2), Type2>::value, "");
	}

	T Compute(float normalizedTime, std::mt19937 & random) const override
	{
		return Compute(normalizedTime,
			std::generate_canonical<float, std::numeric_limits<float>::digits>(random));
	}

	T Compute(float normalizedTime, float amount) const override
	{
		return Details::Particles::ParticleCurveLerp<T>()(
			curve1.Compute(normalizedTime), curve2.Compute(normalizedTime), amount);
	}

	float GenerateVariance(std::mt19937 & random) const override
	{
		return std::generate_canonical<float, std::numeric_limits<float>::digits>(random);
	}
};

}// namespace Particles
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLEPARAMETERRANDOMCURVES_BD1EDD05_8FD4_44FB_A7F3_720E3833A351_HPP)
