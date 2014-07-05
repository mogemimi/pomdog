//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_UIEVENTCONNECTION_EBC5870C_B234_4780_8CBE_4736866F1926_HPP
#define POMDOG_UIEVENTCONNECTION_EBC5870C_B234_4780_8CBE_4736866F1926_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include <utility>
#include <Pomdog/Utility/Assert.hpp>
#include "SubscribeRequestDispatcher.hpp"

namespace Pomdog {
namespace UI {

class UIView;

namespace Details {

class UIEventConnection final {
public:
	typedef UIView ListenerType;
	typedef SubscribeRequestDispatcher<UIView> DispatcherType;

	UIEventConnection(std::weak_ptr<DispatcherType> const& weakDispatcherIn, std::weak_ptr<ListenerType> const& elementIn)
		: weakDispatcher(weakDispatcherIn)
		, element(elementIn)
	{
		POMDOG_ASSERT(!weakDispatcher.expired());
		POMDOG_ASSERT(!element.expired());
	}
	
	UIEventConnection() = default;
	
	~UIEventConnection()
	{
		Disconnect();
	}
	
	UIEventConnection(UIEventConnection const& connection) = delete;
	UIEventConnection & operator=(UIEventConnection const& connection) = delete;
	
	UIEventConnection(UIEventConnection && connection)
	{
		Disconnect();
		
		POMDOG_ASSERT(weakDispatcher.expired());
		POMDOG_ASSERT(element.expired());
		std::swap(weakDispatcher, connection.weakDispatcher);
		std::swap(element, connection.element);
	}
	
	UIEventConnection & operator=(UIEventConnection && connection)
	{
		Disconnect();
		POMDOG_ASSERT(weakDispatcher.expired());
		POMDOG_ASSERT(element.expired());
		std::swap(weakDispatcher, connection.weakDispatcher);
		std::swap(element, connection.element);
		return *this;
	}
	
private:
	void Disconnect()
	{
		if (element.expired()) {
			return;
		}
		
		if (auto dispatcher = weakDispatcher.lock())
		{
			dispatcher->RemoveChild(element);
		}
		
		weakDispatcher.reset();
		element.reset();
	}
	
private:
	std::weak_ptr<DispatcherType> weakDispatcher;
	std::weak_ptr<ListenerType> element;
};

}// namespace Details
}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_UIEVENTCONNECTION_EBC5870C_B234_4780_8CBE_4736866F1926_HPP)
