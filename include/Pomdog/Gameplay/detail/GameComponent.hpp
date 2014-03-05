//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DETAIL_GAMECOMPONENT_68E8B2BF_A61D_463F_AE84_7D2CE4A8A5F2_HPP
#define POMDOG_DETAIL_GAMECOMPONENT_68E8B2BF_A61D_463F_AE84_7D2CE4A8A5F2_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <limits>
#include "../../Utility/Assert.hpp"

namespace Pomdog {
namespace Details {

struct ComponentBitIndexCounter
{
	template <typename T>
	static T Count()
	{
		static_assert(std::is_unsigned<T>::value, "T is unsigned integer.");
		static T count = 0;
		POMDOG_ASSERT(count < std::numeric_limits<T>::max());
		++count;
		POMDOG_ASSERT(count > 0);
		return count;
	}
};

template <typename T, typename IdentType>
struct GameComponentHashCode
{
	static_assert(!std::is_pointer<T>::value, "T is not pointer.");
	static_assert(std::is_object<T>::value, "T is not object type.");
	static_assert(std::is_unsigned<IdentType>::value, "IdentType is unsigned integer.");

	static IdentType const value;
};

template <typename T, typename IdentType>
IdentType const GameComponentHashCode<T, IdentType>::value = ComponentBitIndexCounter::Count<IdentType>();


template <typename HashCodeType>
class GameComponent
{
public:
	GameComponent() = default;
	GameComponent(GameComponent const&) = default;
	GameComponent(GameComponent &&) = default;

	virtual ~GameComponent() = default;

	GameComponent & operator=(GameComponent const&) = default;
	GameComponent & operator=(GameComponent &&) = default;

	static_assert(std::is_unsigned<HashCodeType>::value, "HashCodeType is unsigned integer.");

	virtual HashCodeType GetHashCode() const = 0;
};


template <class T, typename HashCodeType>
class IntrusiveComponent final: public GameComponent<HashCodeType>
{
public:
	static_assert(std::is_fundamental<T>::value ||
		std::is_class<T>::value || std::is_enum<T>::value, "T is class or enum");
	
	IntrusiveComponent() = default;
	~IntrusiveComponent() = default;
	
	template <typename...Arguments>
	explicit IntrusiveComponent(Arguments && ...arguments)
		: value(std::forward<Arguments>(arguments)...)
	{}

	HashCodeType GetHashCode() const override
	{
		return GameComponentHashCode<T, HashCodeType>::value;
	}
	
	T const& Value() const
	{
		return value;
	}
	
	T & Value()
	{
		return value;
	}

private:
	T value;
};

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_DETAIL_GAMECOMPONENT_68E8B2BF_A61D_463F_AE84_7D2CE4A8A5F2_HPP)
