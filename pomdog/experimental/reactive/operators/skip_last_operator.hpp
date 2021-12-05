// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/Reactive/Observable.hpp"
#include "Pomdog/Experimental/Reactive/Observer.hpp"
#include "Pomdog/Utility/Assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Reactive::Detail {

template <class T>
class SkipLastOperator final
    : public Observer<T>
    , public Observable<T> {
public:
    explicit SkipLastOperator(int countIn)
        : count(countIn)
    {
        POMDOG_ASSERT(count > 0);
        events.reserve(count);
    }

    void Subscribe(const std::shared_ptr<Observer<T>>& observerIn) override
    {
        POMDOG_ASSERT(observerIn);
        observer = observerIn;
    }

    void OnNext(T value) override
    {
        POMDOG_ASSERT(count > 0);
        POMDOG_ASSERT(events.size() <= count);
        if (events.size() != count) {
            events.push_back(std::move(value));
            POMDOG_ASSERT(events.size() <= count);
        }
        else {
            POMDOG_ASSERT(!events.empty());
            auto oldValue = std::move(events.front());
            events.erase(events.begin());
            events.push_back(std::move(value));
            if (observer) {
                observer->OnNext(std::move(oldValue));
            }
        }
    }

    void OnError() override
    {
        if (observer) {
            observer->OnError();
        }
    }

    void OnCompleted() override
    {
        if (observer) {
            observer->OnCompleted();
        }
    }

private:
    std::shared_ptr<Observer<T>> observer;
    std::vector<T> events;
    int count;
};

} // namespace Pomdog::Reactive::Detail
