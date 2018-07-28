// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Utility/StringHelper.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>
#include <cstdarg>

namespace Pomdog {

bool StringHelper::HasPrefix(const std::string& s, const std::string& prefix)
{
    if (s.size() < prefix.size()) {
        return false;
    }
    return (s.compare(0, prefix.size(), prefix) == 0);
}

bool StringHelper::HasSuffix(const std::string& s, const std::string& suffix)
{
    if (suffix.empty()) {
        return true;
    }
    if (s.size() < suffix.size()) {
        return false;
    }
    return (s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0);
}

std::string StringHelper::TrimRight(const std::string& source, char separator)
{
    const auto func = [&](char c) { return c != separator; };
    std::string result(
        std::begin(source), std::find_if(std::rbegin(source), std::rend(source), func).base());
    return result;
}

std::string StringHelper::TrimLeft(const std::string& source, char separator)
{
    const auto func = [&](char c) { return c != separator; };
    std::string result(std::find_if(std::begin(source), std::end(source), func), std::end(source));
    return result;
}

std::string
StringHelper::TrimRight(const std::string& source, std::function<bool(char)> isSeparator)
{
    auto func = std::not_fn(std::move(isSeparator));
    std::string result(
        std::begin(source), std::find_if(std::rbegin(source), std::rend(source), func).base());
    return result;
}

std::string StringHelper::TrimLeft(const std::string& source, std::function<bool(char)> isSeparator)
{
    auto func = std::not_fn(std::move(isSeparator));
    std::string result(std::find_if(std::begin(source), std::end(source), func), std::end(source));
    return result;
}

std::vector<std::string>
StringHelper::Split(const std::string& source, char separator)
{
    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    std::string::size_type end = 0;
    while ((end = source.find(separator, start)) != std::string::npos) {
        tokens.push_back(source.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(source.substr(start));
    return tokens;
}

std::vector<std::string>
StringHelper::Split(const std::string& source, const std::string& separator)
{
    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    std::string::size_type end = 0;
    while ((end = source.find(separator, start)) != std::string::npos) {
        tokens.push_back(source.substr(start, end - start));
        start = end + separator.size();
    }
    tokens.push_back(source.substr(start));
    return tokens;
}

namespace {

std::string UnsafeToFormatString(const char* format, std::va_list arg)
{
    std::va_list copiedArguments;
    va_copy(copiedArguments, arg);

#if defined(_MSC_VER)
    char buffer[2048];
    std::memset(buffer, 0, sizeof(buffer));

    const auto length = vsnprintf_s(buffer, _countof(buffer), format, copiedArguments);
    static_assert(std::is_signed<decltype(length)>::value, "");

    va_end(copiedArguments);

    POMDOG_ASSERT(length > 0);

    std::string result(buffer, length);
    POMDOG_ASSERT(result.size() == static_cast<std::size_t>(length));
#else
#if __cplusplus >= 201103L
    using std::vsnprintf;
#endif
    const auto length = vsnprintf(nullptr, 0, format, copiedArguments);
    static_assert(std::is_signed<decltype(length)>::value, "");

    va_end(copiedArguments);

    POMDOG_ASSERT(length > 0);

    std::string result(length + 1, '\0');
    vsnprintf(&result.front(), result.size(), format, arg);

    POMDOG_ASSERT(result.back() == '\0');
    result.resize(length);
#endif

    return result;
}

} // unnamed namespace

std::string StringHelper::Format(const char* format, ...)
{
    std::va_list arg;
    va_start(arg, format);
    auto result = UnsafeToFormatString(format, arg);
    va_end(arg);
    return result;
}

} // namespace Pomdog
