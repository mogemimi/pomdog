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

template <typename T>
class OptionalUnsigned final {
public:
    static_assert(std::is_unsigned<T>::value, "T is unsigned integer type.");
    T data;

    constexpr OptionalUnsigned() noexcept
        : data(std::numeric_limits<T>::max())
    {
    }

    constexpr OptionalUnsigned(T v) noexcept
        : data(v)
    {
    }

    [[nodiscard]] constexpr bool operator==(const OptionalUnsigned& v) const noexcept
    {
        POMDOG_ASSERT(data != std::numeric_limits<T>::max());
        return data == v.data;
    }

    [[nodiscard]] constexpr bool operator!=(const OptionalUnsigned& v) const noexcept
    {
        POMDOG_ASSERT(data != std::numeric_limits<T>::max());
        return data != v.data;
    }

    [[nodiscard]] const T& operator*() const
    {
        POMDOG_ASSERT(data != std::numeric_limits<T>::max());
        return data;
    }

    [[nodiscard]] T& operator*()
    {
        POMDOG_ASSERT(data != std::numeric_limits<T>::max());
        return data;
    }

    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
        return data != std::numeric_limits<T>::max();
    }
};

} // namespace pomdog::skeletal2d::detail

namespace pomdog::skeletal2d {
using JointIndex = detail::OptionalUnsigned<u8>;
} // namespace pomdog::skeletal2d
