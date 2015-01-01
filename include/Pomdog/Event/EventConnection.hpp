//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_EVENTCONNECTION_8E4EC7A9_3200_414F_8897_B050C17ADB6C_HPP
#define POMDOG_EVENTCONNECTION_8E4EC7A9_3200_414F_8897_B050C17ADB6C_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "detail/SignalBody.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <utility>

namespace Pomdog {

class POMDOG_EXPORT EventConnection {
private:
	std::unique_ptr<Details::SignalsAndSlots::ConnectionBody> body;

public:
	EventConnection() = default;

	EventConnection(EventConnection const& connection);
	EventConnection(EventConnection && connection) = default;
	
	EventConnection & operator=(EventConnection const& connection);
	EventConnection & operator=(EventConnection && connection) = default;
	
	template <typename Function>
	explicit EventConnection(std::unique_ptr<Details::SignalsAndSlots::ConnectionBodyOverride<Function>> && bodyIn)
		: body(std::forward<std::unique_ptr<Details::SignalsAndSlots::ConnectionBodyOverride<Function>>>(bodyIn))
	{}

	void Disconnect();
};

}// namespace Pomdog

#endif // !defined(POMDOG_EVENTCONNECTION_8E4EC7A9_3200_414F_8897_B050C17ADB6C_HPP)
