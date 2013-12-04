//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Event/Event.hpp>

namespace Pomdog {

Event::Event(EventCode const& categoryID)
	: categoryID (categoryID)
{}
//-----------------------------------------------------------------------
EventCode const& Event::GetCode() const
{
	return categoryID;
}

}// namespace Pomdog
