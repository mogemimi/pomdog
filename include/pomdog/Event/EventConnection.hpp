//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EVENTCONNECTION_H
#define POMDOG_EVENTCONNECTION_H

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>

namespace Pomdog {
namespace Details {
namespace EventInternal {

class EventSlot;

}// namespace EventInternal
}// namespace Details

class EventConnection
{
private:
	typedef std::weak_ptr<Details::EventInternal::EventSlot> weak_slot;
	weak_slot slot;

public:
	EventConnection() = default;
	
	virtual ~EventConnection() = default;

	EventConnection(EventConnection const& slot);
	
	EventConnection(EventConnection && connection);

	explicit EventConnection(weak_slot const& slot);
	explicit EventConnection(weak_slot && slot);

	auto operator=(EventConnection const& connection)->EventConnection&;
	auto operator=(EventConnection && connection)->EventConnection&;

	void Disconnect();
};

}// namespace Pomdog

#endif // !defined(POMDOG_DETAIL_EVENTCONNECTION_H)
