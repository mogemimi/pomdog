// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

template <typename To, typename From>
[[nodiscard]] inline constexpr To enum_cast(const From& from) noexcept
{
    static_assert(sizeof(To) == sizeof(From));
    static_assert(std::is_enum_v<To>);

    if constexpr (std::is_enum_v<From>) {
        using Underlying = std::underlying_type_t<From>;
        static_assert(std::is_same_v<std::underlying_type_t<To>, Underlying>);
        static_assert(std::is_fundamental_v<Underlying>);
        static_assert(std::is_integral_v<Underlying>);
    }
    else {
        static_assert(std::is_same_v<std::underlying_type_t<To>, From>);
        static_assert(std::is_fundamental_v<From>);
        static_assert(std::is_integral_v<From>);
    }
    return static_cast<To>(from);
}

template <typename From>
[[nodiscard]] inline constexpr auto to_underlying(const From& from) noexcept -> std::underlying_type_t<From>
{
    static_assert(std::is_enum_v<From>);

    using To = std::underlying_type_t<From>;
    static_assert(sizeof(To) == sizeof(From));

    static_assert(std::is_fundamental_v<To>);
    static_assert(std::is_integral_v<To>);

    return static_cast<To>(from);
}

} // namespace pomdog
