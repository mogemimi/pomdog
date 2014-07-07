//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_COMPONENT_0CB8A002_5CD7_4FA6_BD42_17CE2B72EEFA_HPP
#define POMDOG_COMPONENT_0CB8A002_5CD7_4FA6_BD42_17CE2B72EEFA_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>
#include <type_traits>
#include <utility>
#include "detail/ComponentTypeIndex.hpp"
#include "detail/GameComponent.hpp"

namespace Pomdog {

template <class T>
class Component: public Details::Gameplay::GameComponent {
public:
	static_assert(std::is_class<T>::value, "T is class");

	virtual ~Component() = default;

	static std::uint8_t TypeIndex()
	{
		return Details::Gameplay::ComponentTypeIndex<GameComponent, std::uint8_t>::Index<T>();
	}
};

}// namespace Pomdog

#endif // !defined(POMDOG_COMPONENT_0CB8A002_5CD7_4FA6_BD42_17CE2B72EEFA_HPP)
