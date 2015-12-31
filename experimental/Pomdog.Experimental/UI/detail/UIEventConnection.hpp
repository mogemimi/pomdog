// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "SubscribeRequestDispatcher.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <memory>
#include <utility>

namespace Pomdog {
namespace UI {

class UIElement;

namespace Detail {

class UIEventConnection final {
private:
    typedef SubscribeRequestDispatcher<std::weak_ptr<UIElement>> DispatcherType;

    std::weak_ptr<DispatcherType> weakDispatcher;
    std::weak_ptr<UIElement> weakListener;

public:
    UIEventConnection() = default;

    UIEventConnection(std::weak_ptr<DispatcherType> const& weakDispatcherIn,
        std::weak_ptr<UIElement> const& weakListenerIn)
        : weakDispatcher(weakDispatcherIn)
        , weakListener(weakListenerIn)
    {
        POMDOG_ASSERT(!weakDispatcher.expired());
        POMDOG_ASSERT(!weakListener.expired());
    }

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
        POMDOG_ASSERT(weakListener.expired());
        std::swap(weakDispatcher, connection.weakDispatcher);
        std::swap(weakListener, connection.weakListener);
    }

    UIEventConnection & operator=(UIEventConnection && connection)
    {
        Disconnect();

        POMDOG_ASSERT(weakDispatcher.expired());
        POMDOG_ASSERT(weakListener.expired());
        std::swap(weakDispatcher, connection.weakDispatcher);
        std::swap(weakListener, connection.weakListener);
        return *this;
    }

private:
    void Disconnect()
    {
        if (weakListener.expired()) {
            return;
        }

        if (auto dispatcher = weakDispatcher.lock()) {
            dispatcher->RemoveChild(weakListener);
        }

        weakDispatcher.reset();
        weakListener.reset();
    }
};

} // namespace Detail
} // namespace UI
} // namespace Pomdog
