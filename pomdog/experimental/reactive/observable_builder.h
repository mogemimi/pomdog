// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_host.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/reactive/observable.h"
#include "pomdog/experimental/reactive/observable_base.h"
#include "pomdog/experimental/reactive/observer.h"
#include "pomdog/experimental/reactive/operators/buffer_operator.h"
#include "pomdog/experimental/reactive/operators/debounce_operator.h"
#include "pomdog/experimental/reactive/operators/delay_operator.h"
#include "pomdog/experimental/reactive/operators/distinct_operator.h"
#include "pomdog/experimental/reactive/operators/do_operator.h"
#include "pomdog/experimental/reactive/operators/filter_operator.h"
#include "pomdog/experimental/reactive/operators/first_operator.h"
#include "pomdog/experimental/reactive/operators/last_operator.h"
#include "pomdog/experimental/reactive/operators/map_operator.h"
#include "pomdog/experimental/reactive/operators/merge_operator.h"
#include "pomdog/experimental/reactive/operators/scan_operator.h"
#include "pomdog/experimental/reactive/operators/skip_last_operator.h"
#include "pomdog/experimental/reactive/operators/skip_operator.h"
#include "pomdog/experimental/reactive/operators/take_operator.h"
#include "pomdog/experimental/reactive/operators/timeout_operator.h"
#include "pomdog/experimental/reactive/operators/zip_operator.h"
#include "pomdog/experimental/reactive/subscriber.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::reactive {

using ObservableConnection = std::vector<std::shared_ptr<ObservableBase>>;

template <class T>
class ObservableBuilder final {
public:
    ObservableBuilder(
        const std::shared_ptr<GameHost>& gameHostIn,
        const std::shared_ptr<Observable<T>>& source)
        : gameHost(gameHostIn)
        , back(source)
    {
        POMDOG_ASSERT(back);

        POMDOG_ASSERT(source);
        front.push_back(source);
    }

    ObservableBuilder(
        std::shared_ptr<GameHost>&& gameHostIn,
        std::vector<std::shared_ptr<ObservableBase>>&& frontIn,
        std::shared_ptr<Observable<T>>&& backIn)
        : gameHost(std::move(gameHostIn))
        , back(std::move(backIn))
    {
        POMDOG_ASSERT(back);

        POMDOG_ASSERT(!frontIn.empty());
        front = std::move(frontIn);
    }

    ObservableBuilder<std::vector<T>> Buffer(int count)
    {
        auto buffer = std::make_shared<detail::BufferOperator<T>>(count);
        POMDOG_ASSERT(back);
        back->Subscribe(buffer);

        ObservableBuilder<std::vector<T>> builder(std::move(gameHost), std::move(front), std::move(buffer));
        return builder;
    }

    ObservableBuilder Delay(const Duration& interval)
    {
        auto delay = std::make_shared<detail::DelayOperator<T>>(interval, gameHost);
        POMDOG_ASSERT(back);
        back->Subscribe(delay);

        ObservableBuilder<T> builder(std::move(gameHost), std::move(front), std::move(delay));
        return builder;
    }

    ObservableBuilder Debounce(const Duration& dueTime)
    {
        auto debounce = std::make_shared<detail::DebounceOperator<T>>(dueTime, gameHost);
        POMDOG_ASSERT(back);
        back->Subscribe(debounce);

        ObservableBuilder<T> builder(std::move(gameHost), std::move(front), std::move(debounce));
        return builder;
    }

    ObservableBuilder Distinct()
    {
        auto distinct = std::make_shared<detail::DistinctOperator<T>>();
        POMDOG_ASSERT(back);
        back->Subscribe(distinct);

        ObservableBuilder<T> builder(std::move(gameHost), std::move(front), std::move(distinct));
        return builder;
    }

    ObservableBuilder Do(std::function<void(const T&)>&& onNext)
    {
        POMDOG_ASSERT(onNext);
        return Do(std::move(onNext), nullptr, nullptr);
    }

    ObservableBuilder Do(
        std::function<void(const T&)>&& onNext,
        std::function<void()>&& onError)
    {
        POMDOG_ASSERT(onNext);
        POMDOG_ASSERT(onError);
        return Do(std::move(onNext), std::move(onError), nullptr);
    }

    ObservableBuilder Do(
        std::function<void(const T&)>&& onNext,
        std::function<void()>&& onError,
        std::function<void()>&& onCompleted)
    {
        auto doOperator = std::make_shared<detail::DoOperator<T>>(
            std::move(onNext), std::move(onError), std::move(onCompleted));
        POMDOG_ASSERT(back);
        back->Subscribe(doOperator);

        ObservableBuilder<T> builder(std::move(gameHost), std::move(front), std::move(doOperator));
        return builder;
    }

    ObservableBuilder Filter(std::function<bool(const T& value)>&& filter)
    {
        auto filterOperator = std::make_shared<detail::FilterOperator<T>>(std::move(filter));
        POMDOG_ASSERT(back);
        back->Subscribe(filterOperator);

        ObservableBuilder<T> builder(std::move(gameHost), std::move(front), std::move(filterOperator));
        return builder;
    }

    ObservableBuilder First()
    {
        auto firstOperator = std::make_shared<detail::FirstOperator<T>>();
        POMDOG_ASSERT(back);
        back->Subscribe(firstOperator);

        ObservableBuilder<T> builder(std::move(gameHost), std::move(front), std::move(firstOperator));
        return builder;
    }

    ObservableBuilder Last()
    {
        auto lastOperator = std::make_shared<detail::LastOperator<T>>();
        POMDOG_ASSERT(back);
        back->Subscribe(lastOperator);

        ObservableBuilder<T> builder(std::move(gameHost), std::move(front), std::move(lastOperator));
        return builder;
    }

    template <typename TResult>
    auto Map(std::function<TResult(T&& value)>&& map)
        -> ObservableBuilder<TResult>
    {
        auto mapOperator = std::make_shared<detail::MapOperator<T, TResult>>(std::move(map));
        POMDOG_ASSERT(back);
        back->Subscribe(mapOperator);

        ObservableBuilder<TResult> builder(std::move(gameHost), std::move(front), std::move(mapOperator));
        return builder;
    }

    ObservableBuilder Merge(ObservableBuilder&& other)
    {
        auto merge = std::make_shared<detail::MergeOperator<T>>();
        POMDOG_ASSERT(back);
        back->Subscribe(merge);
        other.back->Subscribe(merge);

        front.reserve(front.size() + other.front.size());
        std::move(std::begin(other.front), std::end(other.front), std::back_inserter(front));

        ObservableBuilder<T> builder(std::move(gameHost), std::move(front), std::move(merge));
        return builder;
    }

    ObservableBuilder Scan(std::function<T(const T& accumulation, const T& value)>&& accumulator)
    {
        auto scanOperator = std::make_shared<detail::ScanOperator<T>>(std::move(accumulator));
        POMDOG_ASSERT(back);
        back->Subscribe(scanOperator);

        ObservableBuilder<T> builder(std::move(gameHost), std::move(front), std::move(scanOperator));
        return builder;
    }

    ObservableBuilder Skip(int count)
    {
        auto skip = std::make_shared<detail::SkipOperator<T>>(count);
        POMDOG_ASSERT(back);
        back->Subscribe(skip);

        ObservableBuilder<T> builder(std::move(gameHost), std::move(front), std::move(skip));
        return builder;
    }

    ObservableBuilder SkipLast(int count)
    {
        auto skipLast = std::make_shared<detail::SkipLastOperator<T>>(count);
        POMDOG_ASSERT(back);
        back->Subscribe(skipLast);

        ObservableBuilder<T> builder(std::move(gameHost), std::move(front), std::move(skipLast));
        return builder;
    }

    ObservableBuilder Take(int limit)
    {
        auto take = std::make_shared<detail::TakeOperator<T>>(limit);
        POMDOG_ASSERT(back);
        back->Subscribe(take);

        ObservableBuilder<T> builder(std::move(gameHost), std::move(front), std::move(take));
        return builder;
    }

    ObservableBuilder Timeout(const Duration& dueTime)
    {
        auto timeout = std::make_shared<detail::TimeoutOperator<T>>(dueTime, gameHost);
        POMDOG_ASSERT(back);
        back->Subscribe(timeout);

        ObservableBuilder<T> builder(std::move(gameHost), std::move(front), std::move(timeout));
        return builder;
    }

    template <class TOther, class TResult>
    ObservableBuilder Zip(
        ObservableBuilder<TOther>&& other,
        std::function<TResult(const T& a, const TOther& b)>&& zipper)
    {
        auto zipOperator = std::make_shared<detail::ZipOperator<T, TOther, TResult>>(std::move(zipper));
        POMDOG_ASSERT(back);
        back->Subscribe(zipOperator->GetObserver1());
        auto otherFront = other.Subscribe(zipOperator->GetObserver2());

        front.reserve(front.size() + otherFront.size() + 1);
        std::move(std::begin(otherFront), std::end(otherFront), std::back_inserter(front));
        front.push_back(zipOperator);

        ObservableBuilder<T> builder(std::move(gameHost), std::move(front), std::move(zipOperator));
        return builder;
    }

    ObservableConnection Subscribe(const std::shared_ptr<Observer<T>>& observer)
    {
        POMDOG_ASSERT(observer);
        back->Subscribe(observer);
        back.reset();
        return std::move(front);
    }

    ObservableConnection Subscribe(std::function<void(const T&)>&& onNext)
    {
        POMDOG_ASSERT(onNext);
        return Subscribe(std::move(onNext), nullptr, nullptr);
    }

    ObservableConnection Subscribe(
        std::function<void(const T&)>&& onNext,
        std::function<void()>&& onError)
    {
        POMDOG_ASSERT(onNext);
        POMDOG_ASSERT(onError);
        return Subscribe(std::move(onNext), std::move(onError), nullptr);
    }

    ObservableConnection Subscribe(
        std::function<void(const T&)>&& onNext,
        std::function<void()>&& onError,
        std::function<void()>&& onCompleted)
    {
        POMDOG_ASSERT(onNext);
        POMDOG_ASSERT(back);
        auto subscriber = std::make_shared<Subscriber<T>>(
            std::move(onNext), std::move(onError), std::move(onCompleted));
        back->Subscribe(subscriber);
        back.reset();
        return std::move(front);
    }

private:
    std::shared_ptr<GameHost> gameHost;
    std::vector<std::shared_ptr<ObservableBase>> front;
    std::shared_ptr<Observable<T>> back;
};

template <class TObservable>
auto FromObservable(
    const std::shared_ptr<GameHost>& gameHost,
    std::shared_ptr<TObservable>&& source)
    -> ObservableBuilder<typename TObservable::ObservableType>
{
    using T = typename TObservable::ObservableType;
    static_assert(std::is_base_of<Observable<T>, TObservable>::value, "");
    ObservableBuilder<T> observable(gameHost, source);
    return observable;
}

} // namespace pomdog::reactive
