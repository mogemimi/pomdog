// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Utility/Assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <limits>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Skeletal2D::Detail {

template <typename T, T Denominator>
class CompressedFloat final {
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

    CompressedFloat& operator=(float scalar)
    {
        POMDOG_ASSERT(scalar < Max());
        POMDOG_ASSERT(scalar >= Min());
        data = static_cast<T>(scalar * static_cast<float>(Denominator));
        return *this;
    }

    [[nodiscard]] float ToFloat() const noexcept
    {
        static_assert(Denominator != 0);
        constexpr float scale = 1.0f / Denominator;
        return data * scale;
    }

    [[nodiscard]] static constexpr float Max() noexcept
    {
        static_assert(Denominator != 0);
        return static_cast<float>(std::numeric_limits<T>::max()) / Denominator;
    }

    [[nodiscard]] static constexpr float Min() noexcept
    {
        static_assert(Denominator != 0);
        return static_cast<float>(std::numeric_limits<T>::min()) / Denominator;
    }

    [[nodiscard]] bool operator<(const CompressedFloat& other) const noexcept
    {
        return data < other.data;
    }
};

} // namespace Pomdog::Skeletal2D::Detail
