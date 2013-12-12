//
//  Copyright (C) 2013 mogemimi.
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

#include "detail/FowardDeclarations.hpp"
#include "../Utility/Noncopyable.hpp"
#include <memory>
#include <functional>

namespace Pomdog {

class EventQueue: Noncopyable
{
public:
	EventQueue();
	~EventQueue();

	EventConnection Connect(std::function<void(Event const&)> const& slot);

	EventConnection Connect(std::function<void(Event const&)> && slot);

	void Enqueue(std::shared_ptr<Event const> const& event);
	void Tick();

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace Pomdog

#endif // !defined(POMDOG_EVENTQUEUE_4A197647_AE4E_42C7_AC78_B5E621DD2DA2_HPP)
