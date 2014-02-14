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
#include <utility>
#include "../Config/Export.hpp"
#include "detail/FowardDeclarations.hpp"
#include "detail/Signal.hpp"

namespace Pomdog {

class POMDOG_EXPORT EventConnection
{
private:
	std::weak_ptr<Details::SignalsAndSlots::ConnectionBody> weakConnectionBody;

public:
	EventConnection() = default;
	~EventConnection() = default;

	EventConnection(EventConnection const& connection) = default;
	EventConnection(EventConnection && connection) = default;
	
	EventConnection & operator=(EventConnection const& connection) = default;
	EventConnection & operator=(EventConnection && connection) = default;
	
	template <typename Function>
	explicit EventConnection(std::weak_ptr<Details::SignalsAndSlots::ConnectionBodyOverride<Function>> && connectionBodyIn)
		: weakConnectionBody(std::forward<
			std::weak_ptr<Details::SignalsAndSlots::ConnectionBodyOverride<Function>>
		>(connectionBodyIn))
	{}

	void Disconnect();
};

}// namespace Pomdog

#endif // !defined(POMDOG_EVENTCONNECTION_8E4EC7A9_3200_414F_8897_B050C17ADB6C_HPP)
