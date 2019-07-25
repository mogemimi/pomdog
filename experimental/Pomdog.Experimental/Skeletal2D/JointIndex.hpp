// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

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

    constexpr OptionalUnsigned() noexcept
        : Data(std::numeric_limits<T>::max())
    {}

    constexpr OptionalUnsigned(T v) noexcept
        : Data(v)
    {}

    constexpr bool operator==(OptionalUnsigned const& v) const noexcept
    {
        POMDOG_ASSERT(Data != std::numeric_limits<T>::max());
        return Data == v.Data;
    }

    constexpr bool operator!=(OptionalUnsigned const& v) const noexcept
    {
        POMDOG_ASSERT(Data != std::numeric_limits<T>::max());
        return Data != v.Data;
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

} // namespace SkeletalAnimation2D
} // namespace Detail

using JointIndex = Detail::SkeletalAnimation2D::OptionalUnsigned<std::uint8_t>;

} // namespace Pomdog
