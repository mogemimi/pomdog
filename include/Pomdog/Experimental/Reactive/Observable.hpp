// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Reactive/ObservableBase.hpp"
#include <memory>

namespace Pomdog::Reactive {

template <class T>
class Observer;

template <class T>
class Observable : public ObservableBase {
public:
    using ObservableType = T;

    virtual ~Observable() = default;

    virtual void Subscribe(const std::shared_ptr<Observer<T>>& observer) = 0;
};

} // namespace Pomdog::Reactive
