// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <type_traits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

template <typename T, class Tag>
class POMDOG_EXPORT Tagged final {
public:
    static_assert(std::is_trivially_copyable_v<T>, "You can only use plain-old-data types.");
    static_assert(std::is_standard_layout_v<T>, "You can only use plain-old-data types.");

    using value_type = T;
    using tag_type = Tag;

    T value;

public:
    Tagged() = default;
    Tagged(const Tagged&) = default;
    Tagged(Tagged&&) = default;

    Tagged& operator=(const Tagged&) = default;
    Tagged& operator=(Tagged&&) = default;

    Tagged(const value_type& v)
        : value(v)
    {
    }

    Tagged(value_type&& v)
        : value(std::move(v))
    {
    }

    constexpr const T* Data() const noexcept
    {
        return &value;
    }

    T* Data() noexcept
    {
        return &value;
    }

    constexpr bool operator==(const Tagged& v) const
    {
        return value == v.value;
    }

    constexpr bool operator!=(const Tagged& v) const
    {
        return value != v.value;
    }
};

template <typename T, class Tag, class... Arguments>
Tagged<T, Tag> MakeTagged(Arguments&&... arguments)
{
    static_assert(std::is_trivially_copyable_v<T>, "You can only use plain-old-data types.");
    static_assert(std::is_standard_layout_v<T>, "You can only use plain-old-data types.");
    return Tagged<T, Tag>(std::forward<T>(arguments)...);
}

} // namespace pomdog::detail

namespace std {

template <typename T, class U>
struct hash<pomdog::detail::Tagged<T, U>> {
    std::size_t operator()(const pomdog::detail::Tagged<T, U>& key) const
    {
        return std::hash<T>()(key.value);
    }
};

} // namespace std
