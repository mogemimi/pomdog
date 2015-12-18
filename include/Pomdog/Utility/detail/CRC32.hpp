// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_CRC32_7EBEE5EE_HPP
#define POMDOG_CRC32_7EBEE5EE_HPP

#include "Pomdog/Basic/Export.hpp"
#include <string>
#include <cstdint>
#include <cstddef>
#include <type_traits>

namespace Pomdog {
namespace Detail {

// CRC-32 (Cyclic Redundancy Check for 32bit)
struct POMDOG_EXPORT CRC32 {
    static std::uint32_t ComputeCRC32(void const* data, std::size_t length) noexcept;

    template <typename CharT>
    static std::uint32_t ComputeCRC32(std::basic_string<CharT> const& data) noexcept
    {
        static_assert(std::is_integral<CharT>::value, "You can only use integral types");
        return ComputeCRC32(data.data(), data.length() * sizeof(CharT));
    }
};

} // namespace Detail
} // namespace Pomdof

#endif // POMDOG_CRC32_7EBEE5EE_HPP
