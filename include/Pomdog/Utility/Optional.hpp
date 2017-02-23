// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

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

    constexpr Optional(const NullOptionalType&)
        : data()
        , valid(false)
    {}

    Optional(const Optional&) = default;
    Optional(Optional &&) = default;

    constexpr Optional(const T& v)
        : data(v)
        , valid(true)
    {}

    constexpr Optional(T && v)
        : data(std::move(v))
        , valid(true)
    {}

    Optional & operator=(const NullOptionalType&)
    {
        valid = false;
        data.~T();
        return *this;
    }

    Optional & operator=(const Optional&) = default;
    Optional & operator=(Optional &&) = default;

    Optional & operator=(const T& v)
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

    constexpr const T* operator->() const noexcept
    {
        return POMDOG_CONSTEXPR_ASSERT(valid), &data;
    }

    T* operator->() noexcept
    {
        POMDOG_ASSERT(valid);
        return &data;
    }

    constexpr const T& operator*() const
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

    constexpr const T& value() const
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
