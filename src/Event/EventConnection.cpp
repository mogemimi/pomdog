//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Event/EventConnection.hpp>
#include <utility>
#include "detail/EventSlot.hpp"
#include "detail/EventSlotCollection.hpp"

namespace Pomdog {

//-----------------------------------------------------------------------
EventConnection::EventConnection(weak_slot const& slotIn, weak_slot_collection && collectionIn)
	: slot(slotIn)
	, collection(std::move(collectionIn))
{}
//-----------------------------------------------------------------------
EventConnection::EventConnection(weak_slot && slotIn, weak_slot_collection && collectionIn)
	: slot(std::forward<weak_slot>(slotIn))
	, collection(std::move(collectionIn))
{}
//-----------------------------------------------------------------------
void EventConnection::Disconnect()
{
	auto locked_slot = slot.lock();
	if (!locked_slot) {
		return;
	}

	if (auto locked_collection = collection.lock()) {
		locked_collection->Disconnect(locked_slot.get());
		collection.reset();
	}

	slot.reset();
}

}// namespace Pomdog
