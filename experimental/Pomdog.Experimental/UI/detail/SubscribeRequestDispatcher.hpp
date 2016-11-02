// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include <vector>
#include <utility>

namespace Pomdog {
namespace UI {
namespace Detail {

template <class ListenerType>
class SubscribeRequestDispatcher {
public:
    void AddChild(const ListenerType& child)
    {
        Added.push_back(child);
    }

    void AddChild(ListenerType && child)
    {
        Added.push_back(std::move(child));
    }

    void RemoveChild(const ListenerType& child)
    {
        Removed.push_back(child);
    }

    std::vector<ListenerType> Added;
    std::vector<ListenerType> Removed;
};

} // namespace Detail
} // namespace UI
} // namespace Pomdog
