// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_UIEVENTCONNECTION_EBC5870C_HPP
#define POMDOG_UIEVENTCONNECTION_EBC5870C_HPP

#include "SubscribeRequestDispatcher.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <memory>
#include <utility>

namespace Pomdog {
namespace UI {

class UIView;

namespace Detail {

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

}// namespace Detail
}// namespace UI
}// namespace Pomdog

#endif // POMDOG_UIEVENTCONNECTION_EBC5870C_HPP
