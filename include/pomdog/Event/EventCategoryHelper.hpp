//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EVENTCATEGORYHELPER_H
#define POMDOG_EVENTCATEGORYHELPER_H

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "EventCategoryID.hpp"
#include <string>

namespace Pomdog {

struct EventCategoryHelper
{
	///@~Japanese
	/// @brief 任意の文字列から一意の ID を生成します。
	/// @remarks 半角英数字の文字列のみサポートしています。
	/// また、小文字と大文字は区別されません。
	static EventCategoryID CreateCategoryID(char const* categoryName);
	
	///@~Japanese
	/// @brief 任意の文字列から一意の ID を生成します。
	/// @remarks 半角英数字の文字列のみサポートしています。
	/// また、小文字と大文字は区別されません。
	static EventCategoryID CreateCategoryID(std::string const& categoryName);
};

}// namespace Pomdog

namespace std {

std::string to_string(Pomdog::EventCategoryID const& categoryID);

}// namespace std

#endif // !defined(POMDOG_EVENTCATEGORYHELPER_H)
