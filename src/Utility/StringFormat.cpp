//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Utility/StringFormat.hpp>
#include <cstdarg>
#include <Pomdog/Utility/Assert.hpp>

namespace Pomdog {
namespace {

std::string UnsafeToFormatString(char const* format, std::va_list arg)
{
#if __cplusplus >= 201103L
	using std::vsnprintf;
#endif

	std::va_list copiedArguments;
	va_copy(copiedArguments, arg);

	auto const length = vsnprintf(nullptr, 0, format, copiedArguments);
	static_assert(std::is_signed<decltype(length)>::value, "");
	
	va_end(copiedArguments);
	
	POMDOG_ASSERT(length > 0);
	
	std::string result(length + 1, '\0');
	vsnprintf(&result.front(), result.size(), format, arg);
	
	POMDOG_ASSERT(result.back() == '\0');
	result.resize(length);
	
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
