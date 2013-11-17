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
class EventHandler::Impl
{
public:
	Impl();

public:
	std::shared_ptr<EventSlotCollection> slots;
};
//-----------------------------------------------------------------------
EventHandler::Impl::Impl()
	: slots(std::move(std::make_shared<EventSlotCollection>()))
{}
//-----------------------------------------------------------------------
EventHandler::EventHandler()
	: impl(new Impl())
{}
//-----------------------------------------------------------------------
EventHandler::~EventHandler()
{}
//-----------------------------------------------------------------------
EventConnection EventHandler::Connect(std::function<void(Event const&)> const& slot)
{
	POMDOG_ASSERT(slot);
	POMDOG_ASSERT(impl);
	POMDOG_ASSERT(impl->slots);

	return impl->slots->Connect(slot);
}
//-----------------------------------------------------------------------
void EventHandler::Trigger(std::shared_ptr<Event const> const& event)
{
	POMDOG_ASSERT(event);
	POMDOG_ASSERT(impl);
	POMDOG_ASSERT(impl->slots);

	impl->slots->Trigger(*event);
}

}// namespace Pomdog
