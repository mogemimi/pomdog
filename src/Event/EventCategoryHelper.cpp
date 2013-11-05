//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <pomdog/Event/EventCategoryHelper.hpp>
#include <pomdog/Utility/Assert.hpp>
#include "../Utility/HashingHelper.hpp"
#include <algorithm>
#include <string>

namespace Pomdog {

template <typename T>
static EventCategoryID CreateCategoryID_Impl(T categoryName)
{
	std::string lowerString(categoryName);
	std::transform(std::begin(lowerString), std::end(lowerString), std::begin(lowerString), tolower);

	auto const ident = HashingHelper::Hash(lowerString);
	return EventCategoryID(ident);
}
//-----------------------------------------------------------------------
EventCategoryID EventCategoryHelper::CreateCategoryID(char const* categoryName)
{
	POMDOG_ASSERT(categoryName != nullptr);
	return CreateCategoryID_Impl(categoryName);
}
//-----------------------------------------------------------------------
EventCategoryID EventCategoryHelper::CreateCategoryID(std::string const& categoryName)
{
	POMDOG_ASSERT(!categoryName.empty());
	return CreateCategoryID_Impl(categoryName);
}

}// namespace Pomdog
//-----------------------------------------------------------------------
namespace std {

std::string to_string(Pomdog::EventCategoryID const& categoryID)
{
	return std::to_string(categoryID.value);
}

}// namespace std
