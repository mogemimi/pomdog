//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_PARTICLEPARAMETERRANDOM_03DC4686_AC5B_4B47_9ABB_3693A67A21DE_HPP
#define POMDOG_PARTICLEPARAMETERRANDOM_03DC4686_AC5B_4B47_9ABB_3693A67A21DE_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "ParticleParameter.hpp"
#include "ParticleCurveLerp.hpp"
#include <random>
#include <limits>

namespace Pomdog {
namespace Details {
namespace Particles {

template <typename T>
class ParticleParameterRandom final: public ParticleParameter<T> {
private:
	T min;
	T max;

public:
	template <typename Type1, typename Type2>
	explicit ParticleParameterRandom(Type1 && minIn, Type2 && maxIn)
		: min(std::move(minIn))
		, max(std::move(maxIn))
	{
		static_assert(std::is_convertible<Type1, T>::value, "");
		static_assert(std::is_convertible<Type2, T>::value, "");
	}

	T Compute(std::mt19937 & random) const
	{
		return Details::Particles::ParticleCurveLerp<T>()(min, max,
			std::generate_canonical<float, std::numeric_limits<float>::digits>(random));
	}

	T Compute(float, std::mt19937 & random) const override
	{
		return Compute(random);
	}

	T Compute(float normalizedTime, float amount) const override
	{
		return Details::Particles::ParticleCurveLerp<T>()(min, max, amount);
	}
	
	float GenerateVariance(std::mt19937 & random) const override
	{
		return std::generate_canonical<float, std::numeric_limits<float>::digits>(random);
	}
};

}// namespace Particles
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLEPARAMETERRANDOM_03DC4686_AC5B_4B47_9ABB_3693A67A21DE_HPP)
