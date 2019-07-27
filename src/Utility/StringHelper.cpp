// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Utility/StringHelper.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>
#include <cstdarg>

namespace Pomdog {

bool StringHelper::HasPrefix(const std::string_view& s, const std::string_view& prefix)
{
    if (s.size() < prefix.size()) {
        return false;
    }
    return (s.compare(0, prefix.size(), prefix) == 0);
}

bool StringHelper::HasSuffix(const std::string_view& s, const std::string_view& suffix)
{
    if (suffix.empty()) {
        return true;
    }
    if (s.size() < suffix.size()) {
        return false;
    }
    return (s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0);
}

std::string_view
StringHelper::TrimRight(const std::string_view& source, char separator)
{
    auto v = source;
    auto pos = v.find_last_not_of(separator);
    if (pos != v.npos) {
        POMDOG_ASSERT(v.size() >= (pos + 1));
        v.remove_suffix(v.size() - (pos + 1));
    }
    else {
        v.remove_suffix(v.size());
    }
    return v;
}

std::string_view
StringHelper::TrimLeft(const std::string_view& source, char separator)
{
    auto v = source;
    v.remove_prefix(std::min(v.find_first_not_of(separator), v.size()));
    return v;
}

std::string_view
StringHelper::TrimRight(const std::string_view& source, std::function<bool(char)> isSeparator)
{
    auto func = std::not_fn(std::move(isSeparator));
    auto v = source;
    auto iter = std::find_if(std::rbegin(v), std::rend(v), func);
    if (iter != std::rend(v)) {
        auto pos = std::distance(std::rbegin(v), iter);
        v.remove_suffix(pos);
    }
    else {
        v.remove_suffix(v.size());
    }
    return v;
}

std::string_view
StringHelper::TrimLeft(const std::string_view& source, std::function<bool(char)> isSeparator)
{
    auto func = std::not_fn(std::move(isSeparator));
    auto v = source;
    auto iter = std::find_if(std::begin(source), std::end(source), func);
    if (iter != std::end(v)) {
        auto pos = std::distance(std::begin(v), iter);
        v.remove_prefix(pos);
    }
    else {
        v.remove_prefix(v.size());
    }
    return v;
}

std::vector<std::string_view>
StringHelper::Split(const std::string_view& source, char separator)
{
    std::vector<std::string_view> tokens;
    std::string::size_type start = 0;
    std::string::size_type end = 0;
    while ((end = source.find(separator, start)) != std::string::npos) {
        tokens.push_back(source.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(source.substr(start));
    return tokens;
}

std::vector<std::string_view>
StringHelper::Split(const std::string_view& source, const std::string_view& separator)
{
    std::vector<std::string_view> tokens;
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
