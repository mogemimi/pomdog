// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Event/EventHandler.hpp"
#include "Pomdog/Event/EventConnection.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
EventHandler::EventHandler()
	: signalBody(std::make_shared<Detail::SignalsAndSlots::SignalBody<void(Event const&)>>())
{}
//-----------------------------------------------------------------------
EventConnection EventHandler::Connect(std::function<void(Event const&)> const& slot)
{
	POMDOG_ASSERT(slot);
	POMDOG_ASSERT(this->signalBody);
	return EventConnection{signalBody->Connect(slot)};
}
//-----------------------------------------------------------------------
EventConnection EventHandler::Connect(std::function<void(Event const&)> && slot)
{
	POMDOG_ASSERT(slot);
	POMDOG_ASSERT(this->signalBody);
	return EventConnection{signalBody->Connect(slot)};
}
//-----------------------------------------------------------------------
void EventHandler::Invoke(Event && event)
{
	POMDOG_ASSERT(this->signalBody);
	signalBody->operator()(std::move(event));
}
//-----------------------------------------------------------------------
}// namespace Pomdog
