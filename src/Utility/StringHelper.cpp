// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#include "Pomdog/Utility/StringHelper.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cstdarg>

namespace Pomdog {

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
