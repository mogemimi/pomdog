// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Reactive/Observable.hpp"
#include "Pomdog/Reactive/Observer.hpp"
#include "Pomdog/Reactive/Subscriber.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <deque>
#include <functional>
#include <memory>
#include <utility>

namespace Pomdog {
namespace Reactive {
namespace Detail {

template <class T1, class T2, class TResult>
class ZipOperator final : public Observable<TResult> {
public:
    explicit ZipOperator(std::function<TResult(const T1&, const T2&)>&& zipperIn)
        : zipper(std::move(zipperIn))
        , isStopped(false)
    {
        source1 = std::make_shared<Subscriber<T1>>(
            [this](const T1& value) {
                if (isStopped) {
                    return;
                }
                this->values1.push_back(value);
                this->OnNext();
            },
            [this] { this->OnError(); },
            [this] { this->OnCompleted(); });

        source2 = std::make_shared<Subscriber<T2>>(
            [this](const T2& value) {
                if (isStopped) {
                    return;
                }
                this->values2.push_back(value);
                this->OnNext();
            },
            [this] { this->OnError(); },
            [this] { this->OnCompleted(); });
    }

    void Subscribe(const std::shared_ptr<Observer<TResult>>& observerIn) override
    {
        POMDOG_ASSERT(observerIn);
        observer = observerIn;
    }

    void OnNext()
    {
        POMDOG_ASSERT(!isStopped);
        if (values1.empty() || values2.empty()) {
            return;
        }
        POMDOG_ASSERT(zipper);
        auto value1 = std::move(values1.front());
        auto value2 = std::move(values2.front());
        values1.pop_front();
        values2.pop_front();
        if (observer) {
            observer->OnNext(zipper(std::move(value1), std::move(value2)));
        }
    }

    void OnError()
    {
        if (isStopped) {
            return;
        }
        isStopped = true;
        if (observer) {
            observer->OnError();
        }
    }

    void OnCompleted()
    {
        if (isStopped) {
            return;
        }
        isStopped = true;
        if (observer) {
            observer->OnCompleted();
        }
    }

    const std::shared_ptr<Subscriber<T1>>& GetObserver1()
    {
        return source1;
    }

    const std::shared_ptr<Subscriber<T2>>& GetObserver2()
    {
        return source2;
    }

private:
    std::shared_ptr<Observer<TResult>> observer;
    std::function<TResult(const T1&, const T2&)> zipper;
    std::shared_ptr<Subscriber<T1>> source1;
    std::shared_ptr<Subscriber<T2>> source2;
    std::deque<T1> values1;
    std::deque<T2> values2;
    bool isStopped;
};

} // namespace Detail
} // namespace Reactive
} // namespace Pomdog
