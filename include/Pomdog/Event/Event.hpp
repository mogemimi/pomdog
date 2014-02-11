//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EVENT_56ECB270_07B0_4678_BE7E_FF46C5D1A02B_HPP
#define POMDOG_EVENT_56ECB270_07B0_4678_BE7E_FF46C5D1A02B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <type_traits>
#include <utility>
#include <memory>
#include <string>
#include "../Config/Export.hpp"
#include "../Utility/Assert.hpp"
#include "detail/EventArguments.hpp"

namespace Pomdog {

///@~Japanese
/// @brief イベントです。
class POMDOG_EXPORT Event
{
public:
	Event() = delete;
	Event(Event const&) = delete;
	Event(Event &&) = default;
	
	~Event() = default;

	Event& operator=(Event const&) = delete;
	Event& operator=(Event &&) = default;

	explicit Event(char const* arguments)
		: Event(std::string{arguments})
	{}

	template <typename T>
	explicit Event(T && arguments)
	{
		static_assert(std::is_object<typename std::remove_reference<T>::type>::value, "T is object type.");
		static_assert(!std::is_pointer<T>::value, "pointer type is not supported.");

		typedef Details::EventArgumentsContainer<typename std::remove_reference<T>::type> Container;

		static_assert(std::is_base_of<Details::EventArguments, Container>::value,
			"Container is not a base class of 'EventArguments'");

		///@todo
		/// In C++11:
		///    std::unique_ptr<Container>(new Container(...))
		///
		/// In C++14 or later:
		///    std::make_unique<Container>(...)
		data = std::move(std::unique_ptr<Container>(new Container(std::forward<T>(arguments))));
		//data = std::make_unique<Container>(std::forward<T>(arguments));
	}

	template <class T>
	bool Is() const
	{
		static_assert(!std::is_reference<T>::value, "reference type is not supported.");
		static_assert(!std::is_pointer<T>::value, "pointer type is not supported.");
		static_assert(std::is_object<T>::value, "T is object type.");

		POMDOG_ASSERT(data);
		return data && (data->GetHashCode() == Details::EventComponentHashCode<T>::value);
	}

	template <class T>
	T const* As() const
	{
		static_assert(!std::is_reference<T>::value, "reference type is not supported.");
		static_assert(!std::is_pointer<T>::value, "pointer type is not supported.");
		static_assert(std::is_object<T>::value, "T is object type.");

		typedef Details::EventArgumentsContainer<T> Container;

		static_assert(std::is_base_of<Details::EventArguments, Container>::value,
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
	std::unique_ptr<Details::EventArguments> data;
};

}// namespace Pomdog

#endif // !defined(POMDOG_EVENT_56ECB270_07B0_4678_BE7E_FF46C5D1A02B_HPP)
