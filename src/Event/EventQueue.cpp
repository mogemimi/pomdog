//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Event/EventQueue.hpp>
#include <algorithm>
#include <vector>
#include <mutex>
#include <utility>
#include <Pomdog/Utility/Assert.hpp>
#include "Event.hpp"
#include "detail/EventSlot.hpp"
#include "detail/EventSlotCollection.hpp"

namespace Pomdog {

typedef std::function<void(Event const&)> function_type;

using Details::EventInternal::EventSlot;
using Details::EventInternal::EventSlotCollection;

//-----------------------------------------------------------------------
class EventQueue::Impl
{
public:
	Impl();

	void EnqueueEvent(Event && event);
	
	void Tick();

public:
	std::vector<Event const> events;
	std::shared_ptr<EventSlotCollection> slots;

	std::recursive_mutex notificationProtection;
};
//-----------------------------------------------------------------------
EventQueue::Impl::Impl()
	: slots(std::move(std::make_shared<EventSlotCollection>()))
{}
//-----------------------------------------------------------------------
void EventQueue::Impl::EnqueueEvent(Event && event)
{
	POMDOG_ASSERT(slots);

	std::lock_guard<std::recursive_mutex> lock(notificationProtection);

	events.emplace_back(std::move(event));
}
//-----------------------------------------------------------------------
void EventQueue::Impl::Tick()
{
	POMDOG_ASSERT(slots);

	std::vector<Event const> notifications;
	{
		std::lock_guard<std::recursive_mutex> lock(notificationProtection);

		std::swap(notifications, events);
	}

	for (auto & event: notifications) {
		slots->Invoke(event);
	}
}
//-----------------------------------------------------------------------
EventQueue::EventQueue()
	: impl(new Impl())
{}
//-----------------------------------------------------------------------
EventQueue::~EventQueue() = default;
//-----------------------------------------------------------------------
EventConnection EventQueue::Connect(std::function<void(Event const&)> const& slot)
{
	POMDOG_ASSERT(slot);
	POMDOG_ASSERT(impl);
	POMDOG_ASSERT(impl->slots);

	return impl->slots->Connect(slot);
}
//-----------------------------------------------------------------------
EventConnection EventQueue::Connect(std::function<void(Event const&)> && slot)
{
	POMDOG_ASSERT(slot);
	POMDOG_ASSERT(impl);
	POMDOG_ASSERT(impl->slots);

	return impl->slots->Connect(slot);
}
//-----------------------------------------------------------------------
void EventQueue::Enqueue(Event && event)
{
	POMDOG_ASSERT(impl);
	impl->EnqueueEvent(std::move(event));
}
//-----------------------------------------------------------------------
void EventQueue::Tick()
{
	POMDOG_ASSERT(impl);
	impl->Tick();
}

}// namespace Pomdog
