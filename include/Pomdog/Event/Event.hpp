// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EVENT_56ECB270_07B0_4678_BE7E_FF46C5D1A02B_HPP
#define POMDOG_EVENT_56ECB270_07B0_4678_BE7E_FF46C5D1A02B_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "detail/EventArguments.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <type_traits>
#include <utility>
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT Event {
public:
	Event() = delete;
	Event(Event const&) = delete;
	Event(Event &&) = default;

	Event& operator=(Event const&) = delete;
	Event& operator=(Event &&) = default;

	template <typename T>
	explicit Event(T && arguments)
	{
		static_assert(!std::is_same<Event, typename std::remove_reference<T>::type>::value, "T is not Event type.");
		static_assert(std::is_object<typename std::remove_reference<T>::type>::value, "T is object type.");
		static_assert(!std::is_pointer<T>::value, "pointer type is not supported.");

		typedef Detail::EventArgumentsContainer<typename std::remove_reference<T>::type> Container;

		static_assert(std::is_base_of<Detail::EventArguments, Container>::value,
			"Container is not a base class of 'EventArguments'");

		data = std::make_unique<Container>(std::forward<T>(arguments));
	}

	template <class T>
	bool Is() const
	{
		static_assert(!std::is_reference<T>::value, "reference type is not supported.");
		static_assert(!std::is_pointer<T>::value, "pointer type is not supported.");
		static_assert(std::is_object<T>::value, "T is object type.");

		POMDOG_ASSERT(data);
		return data && (data->HashCode() == Detail::EventComponentHashCode<T>::value);
	}

	template <class T>
	T const* As() const
	{
		static_assert(!std::is_reference<T>::value, "reference type is not supported.");
		static_assert(!std::is_pointer<T>::value, "pointer type is not supported.");
		static_assert(std::is_object<T>::value, "T is object type.");

		typedef Detail::EventArgumentsContainer<T> Container;

		static_assert(std::is_base_of<Detail::EventArguments, Container>::value,
			"T is not a base class of 'EventArguments'.");

		POMDOG_ASSERT(data);

		if (auto p = dynamic_cast<Container const*>(data.get()))
		{
			POMDOG_ASSERT(Is<T>());
			return &p->data;
		}
		return nullptr;
	}

private:
	std::unique_ptr<Detail::EventArguments> data;
};

}// namespace Pomdog

#endif // !defined(POMDOG_EVENT_56ECB270_07B0_4678_BE7E_FF46C5D1A02B_HPP)
