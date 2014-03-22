//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Event/EventQueue.hpp>
#include <algorithm>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Event/detail/SignalBody.hpp>
#include <Pomdog/Event/EventConnection.hpp>

namespace Pomdog {
//-----------------------------------------------------------------------
EventQueue::EventQueue()
	: signalBody(std::move(std::make_shared<SignalType>()))
{}
//-----------------------------------------------------------------------
EventConnection EventQueue::Connect(std::function<void(Event const&)> const& slot)
{
	POMDOG_ASSERT(slot);
	POMDOG_ASSERT(signal);
	return EventConnection{signalBody->Connect(slot)};
}
//-----------------------------------------------------------------------
EventConnection EventQueue::Connect(std::function<void(Event const&)> && slot)
{
	POMDOG_ASSERT(slot);
	POMDOG_ASSERT(signal);
	return EventConnection{signalBody->Connect(slot)};
}
//-----------------------------------------------------------------------
void EventQueue::Enqueue(Event && event)
{
	std::lock_guard<std::recursive_mutex> lock(notificationProtection);
	events.emplace_back(std::move(event));
}
//-----------------------------------------------------------------------
void EventQueue::Tick()
{
	POMDOG_ASSERT(signal);

	std::vector<Event> notifications;
	{
		std::lock_guard<std::recursive_mutex> lock(notificationProtection);

		std::swap(notifications, events);
	}

	for (auto & event: notifications) {
		signalBody->operator()(event);
	}
}
//-----------------------------------------------------------------------
}// namespace Pomdog
