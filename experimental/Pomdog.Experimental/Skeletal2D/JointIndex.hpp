// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_JOINTINDEX_F35DB555_HPP
#define POMDOG_JOINTINDEX_F35DB555_HPP

#include "Pomdog/Utility/Assert.hpp"
#include <cstdint>
#include <type_traits>
#include <limits>

namespace Pomdog {
namespace Detail {
namespace SkeletalAnimation2D {

template <typename T>
class OptionalUnsigned final {
public:
    static_assert(std::is_unsigned<T>::value, "T is unsigned integer type.");
    T Data;

    // Constructors:
    constexpr OptionalUnsigned()
        : Data(std::numeric_limits<T>::max())
    {}

    constexpr OptionalUnsigned(T v)
        : Data(v)
    {}

    // Binary operators:
    constexpr bool operator==(OptionalUnsigned const& v) const
    {
        return POMDOG_CONSTEXPR_ASSERT(Data != std::numeric_limits<T>::max()),
            Data == v.Data;
    }

    constexpr bool operator!=(OptionalUnsigned const& v) const
    {
        return POMDOG_CONSTEXPR_ASSERT(Data != std::numeric_limits<T>::max()),
            Data != v.Data;
    }

    T const& operator*() const
    {
        POMDOG_ASSERT(Data != std::numeric_limits<T>::max());
        return Data;
    }

    T & operator*()
    {
        POMDOG_ASSERT(Data != std::numeric_limits<T>::max());
        return Data;
    }

    constexpr explicit operator bool() const noexcept
    {
        return Data != std::numeric_limits<T>::max();
    }
};

}// namespace SkeletalAnimation2D
}// namespace Detail

using JointIndex = Detail::SkeletalAnimation2D::OptionalUnsigned<std::uint8_t>;

}// namespace Pomdog

#endif // POMDOG_JOINTINDEX_F35DB555_HPP
