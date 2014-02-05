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

#include <typeinfo>
#include <type_traits>
#include <utility>
#include "../../Config/FundamentalTypes.hpp"

namespace Pomdog {
namespace Details {

template <class T>
struct EventComponentTypeID
{
	static std::size_t const value;
};

template <class T>
std::size_t const EventComponentTypeID<T>::value = typeid(T).hash_code();

///@~Japanese
/// @brief イベントに含まれるイベントデータです。
/// @code
///	struct CollisionEvent
///	{
///		std::uint32_t actorId;
///		float x, y;
///	};
///
///	Event a(int{0});
///	Event b(CollisionEvent{1234, 3.0f, 4.0f});
///
///	std::cout << "arguments in a: " << a->Is<CollisionEvent>() << std::endl;
///	std::cout << "arguments in b: " << b->Is<CollisionEvent>() << std::endl;
///
///	if (auto collision = b->As<CollisionEvent>())
/// {
///		std::cout << "actorId = " << collision->actorId << std::endl
///			<< "x = " << collision->x << std::endl
///			<< "y = " << collision->y << std::endl;
///	}
/// @endcode
class EventArguments
{
public:
	virtual ~EventArguments() = default;
	
	virtual std::size_t GetHashCode() const = 0;
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
	
	std::size_t GetHashCode() const override
	{
		return EventComponentTypeID<T>::value;
	}

	T data;
};

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_DETAIL_EVENTARGUMENTS_9EA2B1CE_A235_4143_AE93_18447FB0684A_HPP)
