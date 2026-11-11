// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d::detail {

template <typename T, T Denominator>
class CompressedFloat final {
public:
    static_assert(std::is_integral<T>::value, "");

    T data;

    CompressedFloat() = default;
    CompressedFloat(f32 scalar)
        : data(scalar * Denominator)
    {
        POMDOG_ASSERT(scalar < max());
        POMDOG_ASSERT(scalar >= min());
    }

    CompressedFloat& operator=(f32 scalar)
    {
        POMDOG_ASSERT(scalar < max());
        POMDOG_ASSERT(scalar >= min());
        data = static_cast<T>(scalar * static_cast<f32>(Denominator));
        return *this;
    }

    [[nodiscard]] f32 toFloat() const noexcept
    {
        static_assert(Denominator != 0);
        constexpr f32 scale = 1.0f / Denominator;
        return data * scale;
    }

    [[nodiscard]] static constexpr f32 max() noexcept
    {
        static_assert(Denominator != 0);
        return static_cast<f32>(std::numeric_limits<T>::max()) / Denominator;
    }

    [[nodiscard]] static constexpr f32 min() noexcept
    {
        static_assert(Denominator != 0);
        return static_cast<f32>(std::numeric_limits<T>::min()) / Denominator;
    }

    [[nodiscard]] bool operator<(const CompressedFloat& other) const noexcept
    {
        return data < other.data;
    }
};

} // namespace pomdog::skeletal2d::detail
