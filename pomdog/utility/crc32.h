// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::hash {

// CRC-32 (Cyclic Redundancy Check for 32bit)

[[nodiscard]] POMDOG_EXPORT std::uint32_t
computeCRC32(const std::uint8_t* data, std::size_t length) noexcept;

template <typename CharT>
[[nodiscard]] POMDOG_EXPORT std::uint32_t
computeCRC32(const std::basic_string<CharT>& data) noexcept
{
    static_assert(std::is_integral<CharT>::value, "You can only use integral types");
    return computeCRC32(reinterpret_cast<const std::uint8_t*>(data.data()), data.length() * sizeof(CharT));
}

} // namespace pomdog::hash
