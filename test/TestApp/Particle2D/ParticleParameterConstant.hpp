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

#include <utility>
#include "ParticleParameter.hpp"

namespace Pomdog {

template <typename T>
class ParticleParameterConstant : public ParticleParameter<T> {
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
};

}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLEPARAMETERCONSTANT_0CA7D811_75D8_4AB0_9EA0_A2F9B2EDE095_HPP)
