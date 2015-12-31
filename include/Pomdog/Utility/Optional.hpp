// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Utility/Assert.hpp"
#include <type_traits>
#include <utility>

namespace Pomdog {

struct NullOptionalType final {
    struct init { constexpr init() = default; };
    constexpr explicit NullOptionalType(init) noexcept {}
};

constexpr NullOptionalType NullOpt{ NullOptionalType::init{} };

template <typename T>
class Optional final {
private:
    T data;
    bool valid;

public:
    constexpr Optional()
        : data()
        , valid(false)
    {}

    constexpr Optional(NullOptionalType const&)
        : data()
        , valid(false)
    {}

    Optional(Optional const&) = default;
    Optional(Optional &&) = default;

    constexpr Optional(T const& v)
        : data(v)
        , valid(true)
    {}

    constexpr Optional(T && v)
        : data(std::move(v))
        , valid(true)
    {}

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

    constexpr T const* operator->() const noexcept
    {
        return POMDOG_CONSTEXPR_ASSERT(valid), &data;
    }

    T* operator->() noexcept
    {
        POMDOG_ASSERT(valid);
        return &data;
    }

    constexpr T const& operator*() const
    {
        return POMDOG_CONSTEXPR_ASSERT(valid), data;
    }

    T & operator*()
    {
        POMDOG_ASSERT(valid);
        return data;
    }

    constexpr explicit operator bool() const noexcept
    {
        return valid;
    }

    constexpr T const& value() const
    {
        return POMDOG_CONSTEXPR_ASSERT(valid), data;
    }

    T & value()
    {
        POMDOG_ASSERT(valid);
        return data;
    }
};

template <typename T>
inline constexpr Optional<T> MakeOptional(T v)
{
    return Optional<T>(v);
}

} // namespace Pomdog
