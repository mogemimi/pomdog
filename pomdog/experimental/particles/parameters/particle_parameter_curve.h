// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/particles/parameters/particle_curve_key.h"
#include "pomdog/experimental/particles/parameters/particle_curve_lerp.h"
#include "pomdog/experimental/particles/parameters/particle_parameter.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::particles {

template <typename ForwardIterator, typename KeyType>
std::pair<ForwardIterator, ForwardIterator>
binarySearchNearestPoints(ForwardIterator first, ForwardIterator last, const KeyType& value)
{
    //static_assert(std::is_same<typename std::remove_reference<decltype(*first)>::type, T>::value, "");
    POMDOG_ASSERT(first != last);

    auto it = std::lower_bound(first, last, value,
        [](const KeyType& a, const KeyType& b) { return a.timeSeconds < b.timeSeconds; });

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
    std::vector<ParticleCurveKey<T>> keys_;

public:
    ParticleParameterCurve() = delete;

    template <typename InType>
    explicit ParticleParameterCurve(InType&& keysIn)
        : keys_(std::forward<InType>(keysIn))
    {
        static_assert(std::is_convertible<InType, decltype(keys_)>::value, "");
        POMDOG_ASSERT(!keys_.empty());

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
        using CurveKeyType = ParticleCurveKey<T>;

        POMDOG_ASSERT(std::is_sorted(std::begin(keys_), std::end(keys_),
            [](const CurveKeyType& a, const CurveKeyType& b) { return a.timeSeconds < b.timeSeconds; }));
        POMDOG_ASSERT(std::find_if(std::begin(keys_), std::end(keys_),
                          [](const CurveKeyType& p) { return p.timeSeconds < 0 || p.timeSeconds > 1; }) == std::end(keys_));
#endif
    }

    [[nodiscard]] T
    compute(f32 normalizedScale) const
    {
        POMDOG_ASSERT(!keys_.empty());

        ParticleCurveKey<T> key;
        key.timeSeconds = normalizedScale;
        using detail::particles::binarySearchNearestPoints;
        auto pair = binarySearchNearestPoints(std::begin(keys_), std::end(keys_), key);

        if (pair.first == pair.second) {
            return pair.first->value;
        }

        POMDOG_ASSERT(pair.second->timeSeconds > pair.first->timeSeconds);

        auto diffTime = (normalizedScale - pair.first->timeSeconds);
        auto frameTime = pair.second->timeSeconds - pair.first->timeSeconds;

        POMDOG_ASSERT(frameTime != 0.0f);
        auto amount = diffTime / frameTime;

        using detail::particles::ParticleCurveLerp;
        return ParticleCurveLerp<T>()(pair.first->value, pair.second->value, amount);
    }

    [[nodiscard]] T
    compute(f32 normalizedScale, random::Xoroshiro128StarStar&) const override
    {
        return compute(normalizedScale);
    }

    [[nodiscard]] T
    compute(f32 normalizedScale, f32) const override
    {
        return compute(normalizedScale);
    }

    [[nodiscard]] f32
    generateVariance(random::Xoroshiro128StarStar&) const override
    {
        return 1.0f;
    }
};

} // namespace pomdog::detail::particles
