//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_UTILITY_MAKEUNIQUE_962A10F4_ACBD_42F2_B462_0351035F0176_HPP
#define POMDOG_SRC_UTILITY_MAKEUNIQUE_962A10F4_ACBD_42F2_B462_0351035F0176_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <type_traits>
#include <memory>
#include <utility>

namespace Pomdog {
namespace Details {
namespace InternalMakeUnique {

template <typename T, typename... Arguments>
std::unique_ptr<T> MakeUniqueImplement(std::false_type, Arguments&&... arguments)
{
	return std::unique_ptr<T>(new T(std::forward<Arguments>(arguments)...));
}

template <typename T, typename... Arguments>
std::unique_ptr<T> MakeUniqueImplement(std::true_type, Arguments&&... arguments)
{
	static_assert(!std::extent<T>::value, "You can replace with MakeUnique<T[]>(size);");
	using type = typename std::remove_extent<T>::type;
	return std::unique_ptr<T>(new type[sizeof...(Arguments)]{std::forward<Arguments>(arguments)...});
}

}// namespace InternalMakeUnique

template <typename T, typename... Arguments>
std::unique_ptr<T> MakeUnique(Arguments&&... arguments)
{
	static_assert(__cplusplus < 201401L, "In C++14, you can use std::make_unique ;)");
	
	return InternalMakeUnique::MakeUniqueImplement<T>(
		std::is_array<T>(), std::forward<Arguments>(arguments)...);
}

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_UTILITY_MAKEUNIQUE_962A10F4_ACBD_42F2_B462_0351035F0176_HPP)
