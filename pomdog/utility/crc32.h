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

namespace pomdog::detail::crc32 {

// CRC-32 (Cyclic Redundancy Check for 32bit)

[[nodiscard]] POMDOG_EXPORT std::uint32_t
ComputeCRC32(const void* data, std::size_t length) noexcept;

template <typename CharT>
[[nodiscard]] POMDOG_EXPORT std::uint32_t
ComputeCRC32(const std::basic_string<CharT>& data) noexcept
{
    static_assert(std::is_integral<CharT>::value, "You can only use integral types");
    return ComputeCRC32(data.data(), data.length() * sizeof(CharT));
}

} // namespace pomdog::detail::crc32
