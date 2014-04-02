//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EVENTQUEUE_4A197647_AE4E_42C7_AC78_B5E621DD2DA2_HPP
#define POMDOG_EVENTQUEUE_4A197647_AE4E_42C7_AC78_B5E621DD2DA2_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include <functional>
#include <utility>
#include <vector>
#include <mutex>
#include "../Config/Export.hpp"
#include "detail/ForwardDeclarations.hpp"
#include "Event.hpp"

namespace Pomdog {

class POMDOG_EXPORT EventQueue {
public:
	EventQueue();
	EventQueue(EventQueue const&) = delete;
	EventQueue(EventQueue &&) = delete;
	EventQueue & operator=(EventQueue const&) = delete;
	EventQueue & operator=(EventQueue &&) = delete;

	EventConnection Connect(std::function<void(Event const&)> const& slot);

	EventConnection Connect(std::function<void(Event const&)> && slot);

	void Enqueue(Event && event);

	template <typename T, typename...Arguments>
	void Enqueue(Arguments && ...arguments)
	{
		Enqueue(Event{T{std::forward<Arguments>(arguments)...}});
	}

	void Tick();

private:
	typedef Details::SignalsAndSlots::SignalBody<void(Event const&)> SignalType;

	std::vector<Event> events;
	std::shared_ptr<SignalType> signalBody;
	std::recursive_mutex notificationProtection;
};

}// namespace Pomdog

#endif // !defined(POMDOG_EVENTQUEUE_4A197647_AE4E_42C7_AC78_B5E621DD2DA2_HPP)
