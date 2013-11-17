//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EXCEPTION_HPP
#define POMDOG_EXCEPTION_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <stdexcept>
#include <string>
#include <sstream>

namespace Pomdog {
namespace Details {
namespace ExceptionInternal
{
	template <typename T> constexpr
	char const* ToString() { return "Exception"; }

	// Logic errors
	template<> constexpr char const* ToString<std::logic_error>(){ return "Logic error exception"; }
	template<> constexpr char const* ToString<std::domain_error>(){ return "Domain error exception"; }
	template<> constexpr char const* ToString<std::invalid_argument>(){ return "Invalid argument exception"; }
	template<> constexpr char const* ToString<std::length_error>(){ return "Length error exception"; }
	template<> constexpr char const* ToString<std::out_of_range>(){ return "Out of range exception"; }
	
	// Runtime errors
	template<> constexpr char const* ToString<std::runtime_error>(){ return "Runtime error exception"; }
	template<> constexpr char const* ToString<std::range_error>(){ return "Range error exception"; }
	template<> constexpr char const* ToString<std::overflow_error>(){ return "Overflow error exception"; }
	template<> constexpr char const* ToString<std::underflow_error>(){ return "Underflow error exception"; }
	
	template <typename T>
	T CreateException(std::string const& description, std::string const& source, char const* filename, int line)
	{
		static_assert(std::is_base_of<std::exception, T>::value, "T is base of std::exception");

		// examples:
		// > File "filename", line 4 in source
		// > Runtime error exception: description
		std::stringstream ss;
		ss << "File \"" << filename << "\", line " << line
			<< " in " << (source.empty()? source: "?") << std::endl
			<< ExceptionInternal::ToString<T>()
			<< ": " << description;
		
		return T(ss.str());
	}
	
}// namespace ExceptionInternal
}// namespace Details

#if !defined(POMDOG_THROW_EXCEPTION)

/// @addtogroup Framework
/// @{
/// @addtogroup Utility
/// @{

///@~Japanese
/// @brief 例外を投げます。
/// @param exceptionCode ExceptionCode を指定します。
/// @param description エラーの詳細です。
/// @param source エラーの原因となったオブジェクトやエラーの発生源であるメソッド名を指定します。
#define POMDOG_THROW_EXCEPTION(exceptionClass, description, source) \
	throw Pomdog::Details::ExceptionInternal::CreateException< \
		exceptionClass>(description, source, __FILE__, __LINE__)

/// @}
/// @}

#endif // !defined(POMDOG_THROW_EXCEPTION)

}// namespace Pomdog

#endif // !defined(POMDOG_EXCEPTION_HPP)
