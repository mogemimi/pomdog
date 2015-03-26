// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PARTICLEPARAMETERCURVE_75DE11C7_HPP
#define POMDOG_PARTICLEPARAMETERCURVE_75DE11C7_HPP

#include "ParticleParameter.hpp"
#include "ParticleCurveKey.hpp"
#include "ParticleCurveLerp.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <vector>
#include <algorithm>
#include <utility>

namespace Pomdog {
namespace Detail {
namespace Particles {

template <typename ForwardIterator, typename KeyType>
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


template <typename T>
class ParticleParameterCurve final: public ParticleParameter<T> {
private:
    std::vector<ParticleCurveKey<T>> keys;

public:
    ParticleParameterCurve() = delete;

    template <typename InType>
    explicit ParticleParameterCurve(InType && keysIn)
        : keys(std::move(keysIn))
    {
        static_assert(std::is_convertible<InType, decltype(keys)>::value, "");
        POMDOG_ASSERT(!keys.empty());

        typedef ParticleCurveKey<T> CurveKeyType;

        POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys),
            [](CurveKeyType const& a, CurveKeyType const& b){ return a.TimeSeconds < b.TimeSeconds; }));
        POMDOG_ASSERT(std::find_if(std::begin(keys), std::end(keys),
            [](CurveKeyType const& p){ return p.TimeSeconds < 0 || p.TimeSeconds > 1; }) == std::end(keys));
    }

    T Compute(float normalizedScale) const
    {
        POMDOG_ASSERT(!keys.empty());

        ParticleCurveKey<T> key;
        key.TimeSeconds = normalizedScale;
        using Detail::Particles::BinarySearchNearestPoints;
        auto pair = BinarySearchNearestPoints(std::begin(keys), std::end(keys), key);

        if (pair.first == pair.second)
        {
            return pair.first->Value;
        }

        POMDOG_ASSERT(pair.second->TimeSeconds > pair.first->TimeSeconds);

        auto diffTime = (normalizedScale - pair.first->TimeSeconds);
        auto frameTime = pair.second->TimeSeconds - pair.first->TimeSeconds;

        POMDOG_ASSERT(frameTime != 0.0f);
        auto amount =  diffTime / frameTime;

        using Detail::Particles::ParticleCurveLerp;
        return ParticleCurveLerp<T>()(pair.first->Value, pair.second->Value, amount);
    }

    T Compute(float normalizedScale, std::mt19937 &) const override
    {
        return Compute(normalizedScale);
    }

    T Compute(float normalizedScale, float) const override
    {
        return Compute(normalizedScale);
    }

    float GenerateVariance(std::mt19937 &) const override
    {
        return 1.0f;
    }
};

}// namespace Particles
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_PARTICLEPARAMETERCURVE_75DE11C7_HPP
