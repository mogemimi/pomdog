//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DETAIL_TAGGED_1B3449F5_9833_496C_A481_71FB7536CBF5_HPP
#define POMDOG_DETAIL_TAGGED_1B3449F5_9833_496C_A481_71FB7536CBF5_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "Pomdog/Config/Export.hpp"
#include <type_traits>
#include <functional>
#include <utility>

namespace Pomdog {
namespace Details {

template <typename T, class Tag>
class POMDOG_EXPORT Tagged final {
public:
	static_assert(std::is_pod<T>::value, "You can only use plain-old-data types");

	typedef T value_type;
	typedef Tag tag_type;

	T value;

public:
	Tagged() = default;
	Tagged(Tagged const&) = default;
	Tagged(Tagged &&) = default;
	
	Tagged & operator=(Tagged const&) = default;
	Tagged & operator=(Tagged &&) = default;

	Tagged(value_type const& v)
		: value(v) {}
		
	Tagged(value_type && v)
		: value(std::move(v)) {}

	constexpr T const* Data() const noexcept
	{
		return &value;
	}

	T* Data() noexcept
	{
		return &value;
	}

	constexpr bool operator==(Tagged const& v) const
	{
		return value == v.value;
	}

	constexpr bool operator!=(Tagged const& v) const
	{
		return value != v.value;
	}
};

template <typename T, class Tag, class...Arguments>
Tagged<T, Tag> MakeTagged(Arguments &&... arguments)
{
	static_assert(std::is_pod<T>::value, "You can only use plain-old-data types");
	return Tagged<T, Tag>(std::forward<T>(arguments)...);
}

}// namespace Details
}// namespace Pomdog

namespace std {

template <typename T, class U>
struct hash<Pomdog::Details::Tagged<T, U>> {
	std::size_t operator()(Pomdog::Details::Tagged<T, U> const& key)
	{
		return std::hash<T>()(key.value);
	}
};

}// namespace std

#endif // !defined(POMDOG_DETAIL_TAGGED_1B3449F5_9833_496C_A481_71FB7536CBF5_HPP)
