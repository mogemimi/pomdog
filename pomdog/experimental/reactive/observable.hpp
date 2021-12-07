// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/Reactive/ObservableBase.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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
