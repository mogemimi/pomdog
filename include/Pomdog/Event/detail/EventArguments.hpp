//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_EVENT_DETAIL_EVENTARGUMENTS_9EA2B1CE_A235_4143_AE93_18447FB0684A_HPP
#define POMDOG_EVENT_DETAIL_EVENTARGUMENTS_9EA2B1CE_A235_4143_AE93_18447FB0684A_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <typeinfo>
#include <type_traits>
#include <utility>
#include <cstddef>

namespace Pomdog {
namespace Details {

template <class T>
class EventComponentHashCode {
public:
	static_assert(!std::is_pointer<T>::value, "T is not pointer.");
	static_assert(std::is_object<T>::value, "T is not object type.");

	static std::size_t const value;
};

template <class T>
std::size_t const EventComponentHashCode<T>::value = typeid(T const*).hash_code();


class EventArguments {
public:
	EventArguments() = default;
	EventArguments(EventArguments const&) = delete;
	EventArguments & operator=(EventArguments const&) = delete;

	virtual ~EventArguments() = default;
	
	virtual std::size_t HashCode() const = 0;
};


template <typename T>
class EventArgumentsContainer final: public EventArguments {
public:
	typedef typename std::remove_reference<T>::type value_type;

	static_assert(!std::is_reference<T>::value, "reference type is not supported.");
	static_assert(!std::is_pointer<T>::value, "pointer type is not supported.");
	static_assert(std::is_object<T>::value, "T is object type.");
	
	template <typename...Arguments>
	explicit EventArgumentsContainer(Arguments &&...argument)
		: data(std::forward<Arguments>(argument)...)
	{}
	
	std::size_t HashCode() const override
	{
		return EventComponentHashCode<T>::value;
	}

	T data;
};

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_EVENT_DETAIL_EVENTARGUMENTS_9EA2B1CE_A235_4143_AE93_18447FB0684A_HPP)
