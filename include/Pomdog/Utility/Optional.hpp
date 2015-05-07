// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_OPTIONAL_D43800AB_HPP
#define POMDOG_OPTIONAL_D43800AB_HPP

#include "Pomdog/Utility/Assert.hpp"
#include <type_traits>
#include <utility>

namespace Pomdog {

struct NullOptionalType final {
    struct init { constexpr init() = default; };
    constexpr explicit NullOptionalType(init) noexcept {}
};

#if defined(_MSC_VER) && _MSC_VER <= 1900
const NullOptionalType NullOpt{ NullOptionalType::init{} };
#else
constexpr NullOptionalType NullOpt{ NullOptionalType::init{} };
#endif

template <typename T>
class Optional final {
private:
    T data;
    bool valid;

public:

#if defined(_MSC_VER) && _MSC_VER <= 1900
    Optional()
        : valid(false)
    {}

    Optional(NullOptionalType const&)
        : valid(false)
    {}
#else
    constexpr Optional()
        : valid(false)
    {}

    constexpr Optional(NullOptionalType const&)
        : valid(false)
    {}
#endif

    Optional(Optional const&) = default;
    Optional(Optional &&) = default;

#if defined(_MSC_VER) && _MSC_VER <= 1900
    Optional(T const& v)
        : data(v)
        , valid(true)
    {}

    Optional(T && v)
        : data(std::move(v))
        , valid(true)
    {}
#else
    constexpr Optional(T const& v)
        : data(v)
        , valid(true)
    {}

    constexpr Optional(T && v)
        : data(std::move(v))
        , valid(true)
    {}
#endif

    Optional & operator=(NullOptionalType const&)
    {
        valid = false;
        data.~T();
        return *this;
    }

    Optional & operator=(Optional const&) = default;
    Optional & operator=(Optional &&) = default;

    Optional & operator=(T const& v)
    {
        this->valid = true;
        this->data = v;
        return *this;
    }

    Optional & operator=(T && v)
    {
        this->valid = true;
        this->data = std::move(v);
        return *this;
    }

#if defined(_MSC_VER) && _MSC_VER <= 1900
    T const* operator->() const noexcept
    {
        return &data;
    }
#else
    constexpr T const* operator->() const noexcept
    {
        return POMDOG_CONSTEXPR_ASSERT(valid), &data;
    }
#endif

    T* operator->() noexcept
    {
        POMDOG_ASSERT(valid);
        return &data;
    }

#if defined(_MSC_VER) && _MSC_VER <= 1900
    T const& operator*() const
    {
        return data;
    }
#else
    constexpr T const& operator*() const
    {
        return POMDOG_CONSTEXPR_ASSERT(valid), data;
    }
#endif

    T & operator*()
    {
        POMDOG_ASSERT(valid);
        return data;
    }

#if defined(_MSC_VER) && _MSC_VER <= 1900
    explicit operator bool() const noexcept
    {
        return valid;
    }
#else
    constexpr explicit operator bool() const noexcept
    {
        return valid;
    }
#endif

#if defined(_MSC_VER) && _MSC_VER <= 1900
    T const& value() const
    {
        return data;
    }
#else
    constexpr T const& value() const
    {
        return POMDOG_CONSTEXPR_ASSERT(valid), data;
    }
#endif

    T & value()
    {
        POMDOG_ASSERT(valid);
        return data;
    }
};

#if defined(_MSC_VER) && _MSC_VER <= 1900
template <typename T>
inline Optional<T> MakeOptional(T v)
{
    return Optional<T>(v);
}
#else
template <typename T>
inline constexpr Optional<T> MakeOptional(T v)
{
    return Optional<T>(v);
}
#endif

} // namespace Pomdog

#endif // POMDOG_OPTIONAL_D43800AB_HPP
