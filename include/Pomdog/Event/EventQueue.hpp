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
#include "../Config/Export.hpp"
#include "detail/FowardDeclarations.hpp"

namespace Pomdog {

class POMDOG_EXPORT EventQueue
{
public:
	EventQueue();
	EventQueue(EventQueue const&) = delete;
	EventQueue(EventQueue &&) = default;
	EventQueue& operator=(EventQueue const&) = delete;
	EventQueue& operator=(EventQueue &&) = default;

	~EventQueue();

	EventConnection Connect(std::function<void(Event const&)> const& slot);

	EventConnection Connect(std::function<void(Event const&)> && slot);

	void Enqueue(Event && event);

	template <typename T>
	void Enqueue(T && arguments)
	{
		Enqueue(std::move(Event{std::forward<typename std::remove_reference<T>::type>(arguments)}));
	}

	void Tick();

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace Pomdog

#endif // !defined(POMDOG_EVENTQUEUE_4A197647_AE4E_42C7_AC78_B5E621DD2DA2_HPP)
