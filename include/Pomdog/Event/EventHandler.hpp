//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EVENTHANDLER_HPP
#define POMDOG_EVENTHANDLER_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "detail/FowardDeclarations.hpp"
#include "../Utility/Noncopyable.hpp"
#include <memory>
#include <functional>

namespace Pomdog {

class EventHandler: Noncopyable
{
public:
	EventHandler();
	virtual ~EventHandler() = default;

	EventConnection Connect(std::function<void(Event const&)> const& slot);

	void Trigger(std::shared_ptr<Event const> const& event);

private:
	std::shared_ptr<Details::EventInternal::EventSlotCollection> slots;
};

}// namespace Pomdog

#endif // !defined(POMDOG_EVENTHANDLER_HPP)
