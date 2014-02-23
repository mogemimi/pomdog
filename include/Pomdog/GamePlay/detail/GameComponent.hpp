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
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace Pomdog {
namespace Details {

template <typename T>
struct GameComponentTypeID
{
	static std::size_t const value;
};

template <typename T>
std::size_t const GameComponentTypeID<T>::value = typeid(T).hash_code();

class GameComponent
{
public:
	GameComponent() = default;
	GameComponent(GameComponent const&) = default;
	GameComponent(GameComponent &&) = default;

	virtual ~GameComponent() = default;

	GameComponent & operator=(GameComponent const&) = default;
	GameComponent & operator=(GameComponent &&) = default;

	virtual std::size_t GetHashCode() const = 0;
};

template <class T>
class IntrusiveComponent final: public GameComponent
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

	std::size_t GetHashCode() const override
	{
		return GameComponentTypeID<T>::value;
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
