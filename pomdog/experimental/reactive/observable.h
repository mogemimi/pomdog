// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/reactive/observable_base.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::reactive {

template <class T>
class Observer;

template <class T>
class Observable : public ObservableBase {
public:
    using ObservableType = T;

    virtual ~Observable() = default;

    virtual void Subscribe(const std::shared_ptr<Observer<T>>& observer) = 0;
};

} // namespace pomdog::reactive
