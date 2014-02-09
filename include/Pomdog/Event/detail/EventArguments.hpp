//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DETAIL_EVENTARGUMENTS_9EA2B1CE_A235_4143_AE93_18447FB0684A_HPP
#define POMDOG_DETAIL_EVENTARGUMENTS_9EA2B1CE_A235_4143_AE93_18447FB0684A_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <type_traits>
#include <utility>
#include "../../Config/FundamentalTypes.hpp"

namespace Pomdog {
namespace Details {

///@~Japanese
/// @brief イベントに含まれるイベントデータです。
class EventArguments
{
public:
	virtual ~EventArguments() = default;
};

template <typename T>
class EventArgumentContainer final: public EventArguments
{
public:
	typedef typename std::remove_reference<T>::type value_type;

	static_assert(!std::is_reference<T>::value, "reference type is not supported.");
	static_assert(!std::is_pointer<T>::value, "pointer type is not supported.");
	static_assert(std::is_object<T>::value, "T is object type.");

	~EventArgumentContainer() override = default;

	template <typename C>
	explicit EventArgumentContainer(C && argument)
		: data(std::forward<C>(argument))
	{}

	T data;
};

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_DETAIL_EVENTARGUMENTS_9EA2B1CE_A235_4143_AE93_18447FB0684A_HPP)
