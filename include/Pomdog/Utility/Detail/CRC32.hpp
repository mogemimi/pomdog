// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>

namespace Pomdog::Detail::CRC32 {

// CRC-32 (Cyclic Redundancy Check for 32bit)

[[nodiscard]] POMDOG_EXPORT
std::uint32_t ComputeCRC32(const void* data, std::size_t length) noexcept;

template <typename CharT>
[[nodiscard]] POMDOG_EXPORT
std::uint32_t ComputeCRC32(const std::basic_string<CharT>& data) noexcept
{
    static_assert(std::is_integral<CharT>::value, "You can only use integral types");
    return ComputeCRC32(data.data(), data.length() * sizeof(CharT));
}

} // namespace Pomdog::Detail::CRC32
