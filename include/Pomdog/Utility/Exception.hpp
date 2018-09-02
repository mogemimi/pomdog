// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include <sstream>
#include <stdexcept>
#include <string>

namespace Pomdog {
namespace Detail {
namespace ExceptionInternal {

template <typename T>
constexpr const char* ToString() noexcept
{
    return "Exception";
}

// Logic errors
template <>
constexpr const char* ToString<std::logic_error>() noexcept
{
    return "Logic error exception";
}
template <>
constexpr const char* ToString<std::domain_error>() noexcept
{
    return "Domain error exception";
}
template <>
constexpr const char* ToString<std::invalid_argument>() noexcept
{
    return "Invalid argument exception";
}
template <>
constexpr const char* ToString<std::length_error>() noexcept
{
    return "Length error exception";
}
template <>
constexpr const char* ToString<std::out_of_range>() noexcept
{
    return "Out of range exception";
}

// Runtime errors
template <>
constexpr const char* ToString<std::runtime_error>() noexcept
{
    return "Runtime error exception";
}
template <>
constexpr const char* ToString<std::range_error>() noexcept
{
    return "Range error exception";
}
template <>
constexpr const char* ToString<std::overflow_error>() noexcept
{
    return "Overflow error exception";
}
template <>
constexpr const char* ToString<std::underflow_error>() noexcept
{
    return "Underflow error exception";
}

template <typename T>
T CreateException(const std::string& description, const std::string& source, const char* filename, int line)
{
    static_assert(std::is_base_of<std::exception, T>::value, "T is base of std::exception");

    // examples:
    // > File "filename", line 4 in source
    // > Runtime error exception: description
    std::stringstream ss;
    ss << "File \"" << filename << "\", line " << line
        << " in " << (source.empty()? "?": source) << std::endl
        << ExceptionInternal::ToString<T>()
        << ": " << description;

    return T(ss.str());
}

} // namespace ExceptionInternal
} // namespace Detail

#if !defined(POMDOG_THROW_EXCEPTION)

#define POMDOG_THROW_EXCEPTION(exceptionClass, description) \
    throw Pomdog::Detail::ExceptionInternal::CreateException< \
        exceptionClass>(description, __func__, __FILE__, __LINE__)

#endif // !defined(POMDOG_THROW_EXCEPTION)

} // namespace Pomdog
