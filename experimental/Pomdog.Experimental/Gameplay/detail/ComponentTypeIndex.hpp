//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_COMPONENTTYPEINDEX_7EECA00A_8F27_4D17_A85A_E6B122008DB3_HPP
#define POMDOG_COMPONENTTYPEINDEX_7EECA00A_8F27_4D17_A85A_E6B122008DB3_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Utility/Assert.hpp"
#include <cstdint>
#include <type_traits>
#include <limits>

namespace Pomdog {
namespace Detail {
namespace Gameplay {

template <typename BaseComponentType, typename IndexType>
class ComponentTypeIndex {
public:
	static_assert(std::is_unsigned<IndexType>::value, "IdentType is unsigned integer.");

	template <typename ComponentType>
	static IndexType Index()
	{
		static_assert(!std::is_pointer<ComponentType>::value, "T is not pointer.");
		static_assert(std::is_object<ComponentType>::value, "T is not object type.");
		static const auto value = IncrementIndex();
		return value;
	}

private:
	static IndexType count;

	static IndexType IncrementIndex()
	{
		POMDOG_ASSERT(count < std::numeric_limits<IndexType>::max());
		++count;
		POMDOG_ASSERT(count > 0);
		return count;
	}
};

template <typename BaseComponentType, typename IndexType>
IndexType ComponentTypeIndex<BaseComponentType, IndexType>::count = 0;

}// namespace Gameplay
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_COMPONENTTYPEINDEX_7EECA00A_8F27_4D17_A85A_E6B122008DB3_HPP)
