// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <bit>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

template <typename T>
[[nodiscard]] inline constexpr int popcount(T x) noexcept
{
#if defined(_MSC_VER) && (_MSC_VER <= 1927)
    if constexpr (sizeof(T) <= sizeof(unsigned int)) {
        return __builtin_popcount(x);
    }
    else {
        return __builtin_popcountll(x);
    }
#else
    return std::popcount(x);
#endif
}

template <typename T>
[[nodiscard]] inline constexpr int countl_zero(T x) noexcept
{
#if defined(_MSC_VER) && (_MSC_VER <= 1927)
#error "not implemented yet"
#else
    return std::countl_zero(x);
#endif
}

template <typename T>
[[nodiscard]] inline constexpr int countr_zero(T x) noexcept
{
#if defined(_MSC_VER) && (_MSC_VER <= 1927)
#error "not implemented yet"
#else
    return std::countr_zero(x);
#endif
}

template <typename T>
[[nodiscard]] inline constexpr int countr_one(T x) noexcept
{
#if defined(_MSC_VER) && (_MSC_VER <= 1927)
#error "not implemented yet"
#else
    return std::countr_one(x);
#endif
}

template <typename T>
[[nodiscard]] inline constexpr bool has_single_bit(T x) noexcept
{
#if defined(_MSC_VER) && (_MSC_VER <= 1927)
#error "not implemented yet"
#else
    return std::has_single_bit(x);
#endif
}

template <typename To, typename From>
[[nodiscard]] inline constexpr To bit_cast(const From& from) noexcept
{
#if defined(__APPLE_CC__)
    return __builtin_bit_cast(To, from);
#else
    return std::bit_cast<To>(from);
#endif
}

} // namespace pomdog
