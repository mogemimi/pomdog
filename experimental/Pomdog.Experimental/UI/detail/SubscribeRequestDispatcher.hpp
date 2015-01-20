//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SUBSCRIBEREQUESTDISPATCHER_B5E9A001_3318_45C5_B257_51344C2E2F36_HPP
#define POMDOG_SUBSCRIBEREQUESTDISPATCHER_B5E9A001_3318_45C5_B257_51344C2E2F36_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Utility/Assert.hpp"
#include <memory>
#include <vector>
#include <utility>

namespace Pomdog {
namespace UI {
namespace Details {

template <class ListenerType>
class SubscribeRequestDispatcher {
public:
	void AddChild(std::shared_ptr<ListenerType> const& child)
	{
		Added.push_back(child);
	}

	void AddChild(std::shared_ptr<ListenerType> && child)
	{
		Added.push_back(std::move(child));
	}

	void RemoveChild(std::weak_ptr<ListenerType> const& child)
	{
		POMDOG_ASSERT(!child.expired());
		Removed.push_back(child);
	}

	std::vector<std::shared_ptr<ListenerType>> Added;
	std::vector<std::weak_ptr<ListenerType>> Removed;
};

}// namespace Details
}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_SUBSCRIBEREQUESTDISPATCHER_B5E9A001_3318_45C5_B257_51344C2E2F36_HPP)
