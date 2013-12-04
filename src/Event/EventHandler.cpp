//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Event/EventHandler.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include "detail/EventSlot.hpp"
#include "detail/EventSlotCollection.hpp"

namespace Pomdog {

using Details::EventInternal::EventSlot;
using Details::EventInternal::EventSlotCollection;

//-----------------------------------------------------------------------
EventHandler::EventHandler()
	: slots(std::make_shared<EventSlotCollection>())
{}
//-----------------------------------------------------------------------
EventConnection EventHandler::Connect(std::function<void(Event const&)> const& slot)
{
	POMDOG_ASSERT(slot);
	POMDOG_ASSERT(slots);

	return slots->Connect(slot);
}
//-----------------------------------------------------------------------
void EventHandler::Trigger(std::shared_ptr<Event const> const& event)
{
	POMDOG_ASSERT(event);
	POMDOG_ASSERT(slots);

	slots->Trigger(*event);
}

}// namespace Pomdog
