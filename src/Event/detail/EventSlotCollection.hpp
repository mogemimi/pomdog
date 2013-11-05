//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DETAIL_EVENTSLOTCOLLECTION_H
#define POMDOG_DETAIL_EVENTSLOTCOLLECTION_H

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <pomdog/Utility/Noncopyable.hpp>
#include <pomdog/Event/EventConnection.hpp>
#include <functional>
#include <memory>

namespace Pomdog {

class Event;

namespace Details {
namespace EventInternal {

class EventSlot;

class EventSlotCollection: Noncopyable, public std::enable_shared_from_this<EventSlotCollection>
{
private:
	typedef Event event_type;
	typedef std::function<void(event_type const&)> function_type;

public:
	EventSlotCollection();
	~EventSlotCollection();

	EventConnection Connect(function_type const& slot);
	void Disconnect(EventSlot const* observer);

	void Trigger(event_type const& event);

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace EventInternal
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_DETAIL_EVENTSLOTCOLLECTION_H)
