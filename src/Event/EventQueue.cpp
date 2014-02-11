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

	void EnqueueEvent(std::shared_ptr<Event const> const&);
	void Tick();

public:
	std::vector<std::shared_ptr<Event const>> events;
	std::shared_ptr<EventSlotCollection> slots;

	std::recursive_mutex notificationProtection;
};
//-----------------------------------------------------------------------
EventQueue::Impl::Impl()
	: slots(std::move(std::make_shared<EventSlotCollection>()))
{}
//-----------------------------------------------------------------------
void EventQueue::Impl::EnqueueEvent(std::shared_ptr<Event const> const& event)
{
	POMDOG_ASSERT(event);
	POMDOG_ASSERT(slots);

	std::lock_guard<std::recursive_mutex> lock(notificationProtection);

	events.push_back(event);
}
//-----------------------------------------------------------------------
void EventQueue::Impl::Tick()
{
	POMDOG_ASSERT(slots);

	std::vector<std::shared_ptr<Event const>> notifications;
	{
		std::lock_guard<std::recursive_mutex> lock(notificationProtection);

		std::swap(notifications, events);
	}

	std::for_each(std::begin(notifications), std::end(notifications), [this](std::shared_ptr<Event const> const& event){
		POMDOG_ASSERT(event);
		slots->Trigger(*event);
	});
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
void EventQueue::Enqueue(std::shared_ptr<Event const> const& event)
{
	POMDOG_ASSERT(impl);
	impl->EnqueueEvent(event);
}
//-----------------------------------------------------------------------
void EventQueue::Tick()
{
	POMDOG_ASSERT(impl);
	impl->Tick();
}

}// namespace Pomdog
