//
//  Copyright (C) 2013 mogemimi.
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

#include "../../Config/FundamentalTypes.hpp"
#include <type_traits>
#include <utility>

namespace Pomdog {
namespace Details {

///@~Japanese
/// @brief イベントに含まれるイベントデータです。
/// @code
///	struct CollisionEvent
///	{
///		std::uint32_t actorId;
///		float x, y;
///	};
///
///	auto a = std::make_shared<Event>(EventCode());
///	auto b = std::make_shared<Event>(EventCode(), CollisionEvent{1234, 3.0f, 4.0f});
///
///	std::cout << "arguments in a: " << a->Has<CollisionEvent>() << std::endl;
///	std::cout << "arguments in b: " << b->Has<CollisionEvent>() << std::endl;
///
///	if (auto args = b->Get<CollisionEvent>())
/// {
///		std::cout << "actorId = " << args->actorId << std::endl
///			<< "x = " << args->x << std::endl
///			<< "y = " << args->y << std::endl;
///	}
/// @endcode
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

	template <typename C>
	explicit EventArgumentContainer(C && argument)
		: data(std::forward<C>(argument))
	{}

	T data;
};

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_DETAIL_EVENTARGUMENTS_9EA2B1CE_A235_4143_AE93_18447FB0684A_HPP)
