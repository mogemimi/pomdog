// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <type_traits>
#include <utility>

namespace Pomdog {
namespace Detail {

///@code
/// struct pixel_tag {};
///
/// template <typename T>
/// using Pixel = TaggedArithmetic<T, pixel_tag>;
///
/// Pixel<int> pixel = 32; // 32px
///@endcode
template <typename T, class PhantomType>
class POMDOG_EXPORT TaggedArithmetic final {
public:
    static_assert(std::is_arithmetic<T>::value, "T is arithmetic type.");
    static_assert(std::is_pod<T>::value, "T is pod type.");

    T value;

public:
    // Constructors:
    TaggedArithmetic() noexcept = default;

    TaggedArithmetic(T v) noexcept
        : value(v)
    {
    }

    // Assignment operators:
    TaggedArithmetic& operator+=(const TaggedArithmetic& v) noexcept
    {
        value += v.value;
        return *this;
    }

    TaggedArithmetic& operator+=(TaggedArithmetic&& v) noexcept
    {
        value += std::move(v.value);
        return *this;
    }

    TaggedArithmetic& operator-=(const TaggedArithmetic& v) noexcept
    {
        value -= v.value;
        return *this;
    }

    TaggedArithmetic& operator-=(TaggedArithmetic&& v) noexcept
    {
        value -= std::move(v.value);
        return *this;
    }

    TaggedArithmetic& operator*=(const TaggedArithmetic& v) noexcept
    {
        value *= v.value;
        return *this;
    }

    TaggedArithmetic& operator*=(TaggedArithmetic&& v) noexcept
    {
        value *= std::move(v.value);
        return *this;
    }

    TaggedArithmetic& operator/=(const TaggedArithmetic& v)
    {
        POMDOG_ASSERT(v.value != 0);
        value /= v.value;
        return *this;
    }

    TaggedArithmetic& operator/=(TaggedArithmetic&& v)
    {
        POMDOG_ASSERT(v.value != 0);
        value /= std::move(v.value);
        return *this;
    }

    TaggedArithmetic& operator*=(const T& scale) noexcept
    {
        value *= scale;
        return *this;
    }

    TaggedArithmetic& operator/=(const T& scale)
    {
        POMDOG_ASSERT(scale != 0);
        value /= scale;
        return *this;
    }

    // Unary operators:
    TaggedArithmetic operator+() const noexcept
    {
        return {+value};
    }

    TaggedArithmetic operator-() const noexcept
    {
        return {-value};
    }

    // Binary operators:
    TaggedArithmetic operator+(const TaggedArithmetic& v) const noexcept
    {
        return {value + v.value};
    }

    TaggedArithmetic operator-(const TaggedArithmetic& v) const noexcept
    {
        return {value - v.value};
    }

    TaggedArithmetic operator*(const TaggedArithmetic& v) const noexcept
    {
        return {value * v.value};
    }

    TaggedArithmetic operator/(const TaggedArithmetic& v) const
    {
        POMDOG_ASSERT(v.value != T{0});
        return {value / v.value};
    }

    TaggedArithmetic operator*(T scale) const noexcept
    {
        return {value * scale};
    }

    TaggedArithmetic operator/(T scale) const
    {
        POMDOG_ASSERT(scale != T{0});
        return {value / scale};
    }

    friend TaggedArithmetic operator*(const T& scale, const TaggedArithmetic& v) noexcept
    {
        return {scale * v.value};
    }

    bool operator==(const TaggedArithmetic& v) const noexcept
    {
        return value == v.value;
    }

    bool operator!=(const TaggedArithmetic& v) const noexcept
    {
        return value != v.value;
    }

    bool operator==(const T& scalar) const noexcept
    {
        return value == scalar;
    }

    bool operator!=(const T& scalar) const noexcept
    {
        return value != scalar;
    }

    bool operator<(const TaggedArithmetic& v) const noexcept
    {
        return value < v.value;
    }

    bool operator>(const TaggedArithmetic& v) const noexcept
    {
        return value > v.value;
    }

    bool operator<=(const TaggedArithmetic& v) const noexcept
    {
        return value <= v.value;
    }

    bool operator>=(const TaggedArithmetic& v) const noexcept
    {
        return value >= v.value;
    }

    bool operator<(const T& scalar) const noexcept
    {
        return value < scalar;
    }

    bool operator>(const T& scalar) const noexcept
    {
        return value > scalar;
    }

    bool operator<=(const T& scalar) const noexcept
    {
        return value <= scalar;
    }

    bool operator>=(const T& scalar) const noexcept
    {
        return value >= scalar;
    }
};

} // namespace Detail
} // namespace Pomdog
