//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_PARTICLEPARAMETERCONSTANT_0CA7D811_75D8_4AB0_9EA0_A2F9B2EDE095_HPP
#define POMDOG_PARTICLEPARAMETERCONSTANT_0CA7D811_75D8_4AB0_9EA0_A2F9B2EDE095_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "ParticleParameter.hpp"
#include <utility>

namespace Pomdog {
namespace Details {
namespace Particles {

template <typename T>
class ParticleParameterConstant final: public ParticleParameter<T> {
private:
	T value;

public:
	template <typename InType>
	explicit ParticleParameterConstant(InType && valueIn)
		: value(std::move(valueIn))
	{
		static_assert(std::is_convertible<InType, T>::value, "");
	}

	T Compute(float, std::mt19937 &) const override
	{
		return value;
	}

	T Compute(float, float) const override
	{
		return value;
	}
	
	float GenerateVariance(std::mt19937 & random) const override
	{
		return 1.0f;
	}
};

}// namespace Particles
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLEPARAMETERCONSTANT_0CA7D811_75D8_4AB0_9EA0_A2F9B2EDE095_HPP)
