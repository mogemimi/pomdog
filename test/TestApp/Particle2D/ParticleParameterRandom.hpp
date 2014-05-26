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
#include <Pomdog/Math/Radian.hpp>
#include <Pomdog/Math/Color.hpp>
#include "ParticleParameter.hpp"

namespace Pomdog {
namespace Details {
namespace Particles {

template <typename T>
struct ParticleRandomDistribution {
	template <typename Random>
	T operator()(T const& min, T const& max, Random & random)
	{
		static_assert(std::is_floating_point<T>::value, "");
		std::uniform_real_distribution<T> distribution(min, max);
		return distribution(random);
	}
};

template <typename T>
struct ParticleRandomDistribution<Radian<T>> {
	template <typename Random>
	Radian<T> operator()(Radian<T> const& min, Radian<T> const& max, Random & random)
	{
		std::uniform_real_distribution<T> distribution(min.value, max.value);
		return distribution(random);
	}
};

template <>
struct ParticleRandomDistribution<Color> {
	template <typename Random>
	Color operator()(Color const& min, Color const& max, Random & random)
	{
		std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
		return Color::Lerp(min, max, distribution(random));
	}
};

}// namespace Particles
}// namespace Details

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

	T Compute(float, std::mt19937 & random) const override
	{
		using Details::Particles::ParticleRandomDistribution;
		ParticleRandomDistribution<T> distribution;
		return distribution(min, max, random);
	}
};

}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLEPARAMETERRANDOM_03DC4686_AC5B_4B47_9ABB_3693A67A21DE_HPP)
