//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <pomdog/Event/Event.hpp>

namespace Pomdog {

Event::Event(EventCategoryID const& categoryID)
	: categoryID (categoryID)
{}
//-----------------------------------------------------------------------
EventCategoryID const& Event::GetCategoryID() const
{
	return categoryID;
}

}// namespace Pomdog
