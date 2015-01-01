//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include <Pomdog/Utility/StringFormat.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <cstdarg>

namespace Pomdog {
namespace {

static std::string UnsafeToFormatString(char const* format, std::va_list arg)
{
	std::va_list copiedArguments;
	va_copy(copiedArguments, arg);

#ifdef POMDOG_COMPILER_MSVC
	char buffer[2048];
	std::memset(buffer, 0, sizeof(buffer));

	auto const length = vsnprintf_s(buffer, _countof(buffer), format, copiedArguments);
	static_assert(std::is_signed<decltype(length)>::value, "");

	va_end(copiedArguments);

	POMDOG_ASSERT(length > 0);

	std::string result(buffer, length);
	POMDOG_ASSERT(result.size() == static_cast<std::size_t>(length));
#else
#if __cplusplus >= 201103L
	using std::vsnprintf;
#endif
	auto const length = vsnprintf(nullptr, 0, format, copiedArguments);
	static_assert(std::is_signed<decltype(length)>::value, "");

	va_end(copiedArguments);

	POMDOG_ASSERT(length > 0);

	std::string result(length + 1, '\0');
	vsnprintf(&result.front(), result.size(), format, arg);

	POMDOG_ASSERT(result.back() == '\0');
	result.resize(length);
#endif

	return std::move(result);
}

}// unnamed namespace
//-----------------------------------------------------------------------
std::string StringFormat(char const* format, ...)
{
	std::va_list arg;
	va_start(arg, format);
	auto result = UnsafeToFormatString(format, arg);
	va_end(arg);
	return std::move(result);
}

}// namespace Pomdog
