// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#if __has_include(<optional>)
#include <optional>
#else
#include <experimental/optional>
#endif

namespace Pomdog {

#if __has_include(<optional>)
template <typename T>
using Optional = std::optional<T>;

constexpr auto NullOpt = std::nullopt;

template <typename T>
inline constexpr Optional<T> MakeOptional(T && v)
{
    return std::make_optional(std::forward<T>(v));
}
#else
template <typename T>
using Optional = std::experimental::optional<T>;

constexpr auto NullOpt = std::experimental::nullopt;

template <typename T>
inline constexpr Optional<T> MakeOptional(T && v)
{
    return std::experimental::make_optional(std::forward<T>(v));
}
#endif

} // namespace Pomdog
