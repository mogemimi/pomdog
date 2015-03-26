// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TAGGED_1B3449F5_HPP
#define POMDOG_TAGGED_1B3449F5_HPP

#include "Pomdog/Basic/Export.hpp"
#include <type_traits>
#include <functional>
#include <utility>

namespace Pomdog {
namespace Detail {

template <typename T, class Tag>
class POMDOG_EXPORT Tagged final {
public:
    static_assert(std::is_pod<T>::value, "You can only use plain-old-data types");

    typedef T value_type;
    typedef Tag tag_type;

    T value;

public:
    Tagged() = default;
    Tagged(Tagged const&) = default;
    Tagged(Tagged &&) = default;

    Tagged & operator=(Tagged const&) = default;
    Tagged & operator=(Tagged &&) = default;

    Tagged(value_type const& v)
        : value(v) {}

    Tagged(value_type && v)
        : value(std::move(v)) {}

    constexpr T const* Data() const noexcept
    {
        return &value;
    }

    T* Data() noexcept
    {
        return &value;
    }

    constexpr bool operator==(Tagged const& v) const
    {
        return value == v.value;
    }

    constexpr bool operator!=(Tagged const& v) const
    {
        return value != v.value;
    }
};

template <typename T, class Tag, class...Arguments>
Tagged<T, Tag> MakeTagged(Arguments &&... arguments)
{
    static_assert(std::is_pod<T>::value, "You can only use plain-old-data types");
    return Tagged<T, Tag>(std::forward<T>(arguments)...);
}

}// namespace Detail
}// namespace Pomdog

namespace std {

template <typename T, class U>
struct hash<Pomdog::Detail::Tagged<T, U>> {
    std::size_t operator()(Pomdog::Detail::Tagged<T, U> const& key)
    {
        return std::hash<T>()(key.value);
    }
};

}// namespace std

#endif // POMDOG_TAGGED_1B3449F5_HPP
