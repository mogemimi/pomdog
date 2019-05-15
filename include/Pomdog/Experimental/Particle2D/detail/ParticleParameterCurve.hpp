// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "ParticleCurveKey.hpp"
#include "ParticleCurveLerp.hpp"
#include "ParticleParameter.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>
#include <utility>
#include <vector>

namespace Pomdog {
namespace Detail {
namespace Particles {

template <typename ForwardIterator, typename KeyType>
std::pair<ForwardIterator, ForwardIterator> BinarySearchNearestPoints(ForwardIterator first, ForwardIterator last, const KeyType& value)
{
    //static_assert(std::is_same<typename std::remove_reference<decltype(*first)>::type, T>::value, "");
    POMDOG_ASSERT(first != last);

    auto it = std::lower_bound(first, last, value,
        [](const KeyType& a, const KeyType& b){ return a.TimeSeconds < b.TimeSeconds; });

    if (it == last) {
        return std::make_pair(std::prev(last), std::prev(last));
    }
    else if (it == first) {
        return std::make_pair(first, first);
    }
    return std::make_pair(std::prev(it), it);
}

template <typename T>
class ParticleParameterCurve final : public ParticleParameter<T> {
private:
    std::vector<ParticleCurveKey<T>> keys;

public:
    ParticleParameterCurve() = delete;

    template <typename InType>
    explicit ParticleParameterCurve(InType&& keysIn)
        : keys(std::move(keysIn))
    {
        static_assert(std::is_convertible<InType, decltype(keys)>::value, "");
        POMDOG_ASSERT(!keys.empty());

#if defined(DEBUG) && !defined(NDEBUG)
        using CurveKeyType = ParticleCurveKey<T>;

        POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys),
            [](const CurveKeyType& a, const CurveKeyType& b){ return a.TimeSeconds < b.TimeSeconds; }));
        POMDOG_ASSERT(std::find_if(std::begin(keys), std::end(keys),
            [](const CurveKeyType& p){ return p.TimeSeconds < 0 || p.TimeSeconds > 1; }) == std::end(keys));
#endif
    }

    T Compute(float normalizedScale) const
    {
        POMDOG_ASSERT(!keys.empty());

        ParticleCurveKey<T> key;
        key.TimeSeconds = normalizedScale;
        using Detail::Particles::BinarySearchNearestPoints;
        auto pair = BinarySearchNearestPoints(std::begin(keys), std::end(keys), key);

        if (pair.first == pair.second) {
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

    T Compute(float normalizedScale, std::mt19937&) const override
    {
        return Compute(normalizedScale);
    }

    T Compute(float normalizedScale, float) const override
    {
        return Compute(normalizedScale);
    }

    float GenerateVariance(std::mt19937&) const override
    {
        return 1.0f;
    }
};

} // namespace Particles
} // namespace Detail
} // namespace Pomdog
