//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_PARTICLEPARAMETERRANDOM_03DC4686_AC5B_4B47_9ABB_3693A67A21DE_HPP
#define POMDOG_PARTICLEPARAMETERRANDOM_03DC4686_AC5B_4B47_9ABB_3693A67A21DE_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <random>
#include "ParticleParameter.hpp"

namespace Pomdog {

template <typename T>
class ParticleParameterRandom final: public ParticleParameter<T> {
private:
	T min;
	T max;

public:
	explicit ParticleParameterRandom(T && minIn, T && maxIn)
		: min(std::move(minIn))
		, max(std::move(maxIn))
	{}

	T Compute(float, std::mt19937 & random) const override
	{
		static_assert(std::is_same<T, float>::value, "");
		std::uniform_real_distribution<float> distribution(min, max);
		return distribution(random);
	}
};

}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLEPARAMETERRANDOM_03DC4686_AC5B_4B47_9ABB_3693A67A21DE_HPP)
