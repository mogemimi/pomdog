//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EVENTCONNECTION_8E4EC7A9_3200_414F_8897_B050C17ADB6C_HPP
#define POMDOG_EVENTCONNECTION_8E4EC7A9_3200_414F_8897_B050C17ADB6C_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include "../Config/Export.hpp"
#include "detail/FowardDeclarations.hpp"

namespace Pomdog {

class POMDOG_EXPORT EventConnection
{
private:
	typedef std::weak_ptr<Details::EventInternal::EventSlot> weak_slot;
	weak_slot slot;

public:
	EventConnection() = default;
	~EventConnection() = default;

	EventConnection(EventConnection const& slot);
	
	EventConnection(EventConnection && connection);

	explicit EventConnection(weak_slot const& slot);
	explicit EventConnection(weak_slot && slot);

	auto operator=(EventConnection const& connection)->EventConnection&;
	auto operator=(EventConnection && connection)->EventConnection&;

	void Disconnect();
};

}// namespace Pomdog

#endif // !defined(POMDOG_EVENTCONNECTION_8E4EC7A9_3200_414F_8897_B050C17ADB6C_HPP)
