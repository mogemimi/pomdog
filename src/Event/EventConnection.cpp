//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Event/EventConnection.hpp>
#include "detail/EventSlot.hpp"
#include <utility>

namespace Pomdog {

//-----------------------------------------------------------------------
EventConnection::EventConnection(EventConnection const& connection)
	: slot(connection.slot)
{}
//-----------------------------------------------------------------------
EventConnection::EventConnection(EventConnection && connection)
	: slot(std::forward<weak_slot>(connection.slot))
{}
//-----------------------------------------------------------------------
EventConnection::EventConnection(weak_slot const& slot_)
	: slot(slot_)
{}
//-----------------------------------------------------------------------
EventConnection::EventConnection(weak_slot && slot_)
	: slot(std::forward<weak_slot>(slot_))
{}
//-----------------------------------------------------------------------
auto EventConnection::operator=(EventConnection const& connection)->EventConnection&
{
	slot = connection.slot;
	return *this;
}
//-----------------------------------------------------------------------
auto EventConnection::operator=(EventConnection && connection)->EventConnection&
{
	slot = std::move(connection.slot);
	return *this;
}
//-----------------------------------------------------------------------
void EventConnection::Disconnect()
{
	if (auto p = slot.lock()) {
		p->Disconnect();
		slot.reset();
	}
}

}// namespace Pomdog
