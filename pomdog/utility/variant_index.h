// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <type_traits>
#include <variant>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Gets the index of the type `T` in the std::variant `VariantType` at compile time.
template <class VariantType, typename T, std::size_t index = 0>
[[nodiscard]] consteval std::size_t variant_index() noexcept
{
    if constexpr (index == std::variant_size_v<VariantType>) {
        return index;
    }
    else if constexpr (std::is_same_v<std::variant_alternative_t<index, VariantType>, T>) {
        return index;
    }
    else {
        return variant_index<VariantType, T, index + 1>();
    }
}

} // namespace pomdog
