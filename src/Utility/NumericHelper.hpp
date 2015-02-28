// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_NUMERICHELPER_9678157F_ED59_4605_BA90_535DD92FC994_HPP
#define POMDOG_NUMERICHELPER_9678157F_ED59_4605_BA90_535DD92FC994_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Utility/Assert.hpp"
#include <limits>
#include <type_traits>

namespace Pomdog {
namespace Detail {

struct NumericHelper {
	template <typename R, typename T>
	static R Cast(T value)
	{
	#if defined(DEBUG) && !defined(NDEBUG)
		static_assert(!std::is_same<R, T>::value, "");
		static_assert(std::is_integral<R>::value, "");
		static_assert(std::is_integral<T>::value, "");

		POMDOG_ASSERT_MESSAGE(value <= std::numeric_limits<R>::max(),
			"The value is larger than numeric_limits<R>::max()");

		if (std::is_signed<T>::value && std::is_unsigned<R>::value) {
			POMDOG_ASSERT_MESSAGE(value >= 0,
				"The value is not unsigned integer");
		}
	#endif
		return static_cast<R>(value);
	}
};

}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_NUMERICHELPER_9678157F_ED59_4605_BA90_535DD92FC994_HPP)
