//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_PARTICLESYSTEM_2AEA7BC5_5CCE_4688_8FAC_D26A97A8F8B7_HPP
#define POMDOG_PARTICLESYSTEM_2AEA7BC5_5CCE_4688_8FAC_D26A97A8F8B7_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>
#include <vector>
#include <Pomdog/Pomdog.hpp>
#include "Particle.hpp"
#include "ParticleEmitter.hpp"

namespace Pomdog {


template <typename T>
class ParticleCurveKey {
public:
	//std::uint8_t NormalizedTime;
	float TimeSeconds;
	T Value;
};

class ParticleColorConstant;

template <typename T>
struct ParticleCurveLerp {
	T operator()(T const& a, T const& b, float amount)
	{
		return MathHelper::Lerp(a, b, amount);
	}
};

template <>
struct ParticleCurveLerp<Color> {
	Color operator()(Color const& a, Color const& b, float amount)
	{
		return Color::Lerp(a, b, amount);
	}
};

template <typename T>
class ParticleCurve {
private:
	std::vector<ParticleCurveKey<T>> keys;
	
public:
	ParticleCurve() = delete;
	
	explicit ParticleCurve(Color const& color)
	{
		keys.push_back({0, color});
	}
	
	explicit ParticleCurve(std::vector<ParticleCurveKey<T>> && keysIn)
		: keys(std::move(keysIn))
	{
		POMDOG_ASSERT(!keys.empty());
		
		typedef ParticleCurveKey<T> CurveKeyType;
		
		POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys),
			[](CurveKeyType const& a, CurveKeyType const& b){ return a.TimeSeconds < b.TimeSeconds; }));
		POMDOG_ASSERT(std::find_if(std::begin(keys), std::end(keys),
			[](CurveKeyType const& p){ return p.TimeSeconds < 0 || p.TimeSeconds > 1; }) == std::end(keys));
	}
	
//	T StartValue() const
//	{
//		POMDOG_ASSERT(!keys.empty());
//		return keys.front().Value;
//	}
	
	template <typename ForwardIterator, typename KeyType>
	static
	std::pair<ForwardIterator, ForwardIterator> BinarySearchNearestPoints(ForwardIterator first, ForwardIterator last, KeyType const& value)
	{
		//static_assert(std::is_same<typename std::remove_reference<decltype(*first)>::type, T>::value, "");
		POMDOG_ASSERT(first != last);

		auto it = std::lower_bound(first, last, value,
			[](KeyType const& a, KeyType const& b){ return a.TimeSeconds < b.TimeSeconds; });

		if (it == last) {
			return std::make_pair(std::prev(last), std::prev(last));
		}
		else if (it == first) {
			return std::make_pair(first, first);
		}
		return std::make_pair(std::prev(it), it);
	}
	
	T Compute(float ratio) const
	{
		POMDOG_ASSERT(!keys.empty());
	
		ParticleCurveKey<T> key;
		key.TimeSeconds = ratio;
		auto pair = BinarySearchNearestPoints(std::begin(keys), std::end(keys), key);
		if (pair.first == pair.second)
		{
			return pair.first->Value;
		}
		
		POMDOG_ASSERT(pair.second->TimeSeconds > pair.first->TimeSeconds);
		
		auto diffTime = (ratio - pair.first->TimeSeconds);
		auto frameTime = pair.second->TimeSeconds - pair.first->TimeSeconds;
	
		POMDOG_ASSERT(frameTime != 0.0f);
		auto amount =  diffTime / frameTime;

		return ParticleCurveLerp<T>()(pair.first->Value, pair.second->Value, amount);
	}
};

class ParticleSystem {
public:
	ParticleSystem();

	void Update(DurationSeconds const& frameDuration);
	
	ParticleEmitter emitter;
	std::vector<Particle> particles;
	Vector2 EmitterPosition;
	
	
private:
	DurationSeconds timer;
};

}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLESYSTEM_2AEA7BC5_5CCE_4688_8FAC_D26A97A8F8B7_HPP)
