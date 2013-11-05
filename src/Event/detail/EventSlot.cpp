//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "EventSlot.hpp"
#include "EventSlotCollection.hpp"
#include <pomdog/Utility/Assert.hpp>

namespace Pomdog {
namespace Details {
namespace EventInternal {

EventSlot::EventSlot(function_type const& invoker, std::weak_ptr<EventSlotCollection> const& slots)
	: invoker(invoker)
	, slots(slots)
{}
//-----------------------------------------------------------------------
void EventSlot::Disconnect()
{
	if (auto p = slots.lock()) {
		p->Disconnect(this);
		slots.reset();
	}
}
//-----------------------------------------------------------------------
void EventSlot::Invoke(event_type const& event)
{
	POMDOG_ASSERT(invoker);
	invoker(event);
}

}// namespace EventInternal
}// namespace Details
}// namespace Pomdog
