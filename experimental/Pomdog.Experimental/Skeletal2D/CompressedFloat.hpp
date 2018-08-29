// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Utility/Assert.hpp"
#include <type_traits>
#include <cstdint>
#include <limits>

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {

template <typename T, T Denominator>
class CompressedFloat {
public:
    static_assert(std::is_integral<T>::value, "");

    T data;

    CompressedFloat() = default;
    CompressedFloat(float scalar)
        : data(scalar * Denominator)
    {
        POMDOG_ASSERT(scalar < Max());
        POMDOG_ASSERT(scalar >= Min());
    }

    CompressedFloat & operator=(float scalar)
    {
        POMDOG_ASSERT(scalar < Max());
        POMDOG_ASSERT(scalar >= Min());
        data = scalar * Denominator;
        return *this;
    }

    float ToFloat() const noexcept
    {
        static_assert(Denominator != 0);
        constexpr float scale = 1.0f / Denominator;
        return data * scale;
    }

    constexpr static float Max() noexcept
    {
        static_assert(Denominator != 0);
        return static_cast<float>(std::numeric_limits<T>::max()) / Denominator;
    }

    constexpr static float Min() noexcept
    {
        static_assert(Denominator != 0);
        return static_cast<float>(std::numeric_limits<T>::min()) / Denominator;
    }

    bool operator<(const CompressedFloat& other) const noexcept
    {
        return data < other.data;
    }
};

} // namespace Skeletal2D
} // namespace Detail
} // namespace Pomdog
