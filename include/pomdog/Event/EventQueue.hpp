//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EVENTQUEUE_HPP
#define POMDOG_EVENTQUEUE_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Utility/Noncopyable.hpp"
#include <memory>
#include <functional>

namespace Pomdog {

class Event;
class EventConnection;

class EventQueue: Noncopyable
{
public:
	EventQueue();
	virtual ~EventQueue();

	EventConnection Connect(std::function<void(Event const&)> const& slot);

	void Enqueue(std::shared_ptr<Event const> const& event);
	void Tick();

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace Pomdog

#endif // !defined(POMDOG_EVENTQUEUE_HPP)
