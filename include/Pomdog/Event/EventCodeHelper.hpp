//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EVENTCODEHELPER_56ECB270_07B0_4678_BE7E_FF46C5D1A02B_HPP
#define POMDOG_EVENTCODEHELPER_56ECB270_07B0_4678_BE7E_FF46C5D1A02B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "EventCode.hpp"
#include <string>

namespace std {

std::string to_string(Pomdog::EventCode const& categoryID);

}// namespace std

namespace Pomdog {

struct EventCodeHelper
{
	///@~Japanese
	/// @brief 任意の文字列から一意の ID を生成します。
	/// @remarks 半角英数字の文字列のみサポートしています。
	/// また、小文字と大文字は区別されません。
	static EventCode CreateCode(char const* categoryName);
	
	///@~Japanese
	/// @brief 任意の文字列から一意の ID を生成します。
	/// @remarks 半角英数字の文字列のみサポートしています。
	/// また、小文字と大文字は区別されません。
	static EventCode CreateCode(std::string const& categoryName);
};

}// namespace Pomdog

#endif // !defined(POMDOG_EVENTCODEHELPER_56ECB270_07B0_4678_BE7E_FF46C5D1A02B_HPP)
