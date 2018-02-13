// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <atomic>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>
#include <vector>

namespace Pomdog {
namespace Concurrency {

template <typename TResult>
class Task;

template <typename TResult>
class TaskCompletionSource;

template <>
class TaskCompletionSource<void>;

class ConcurrencyException final : public std::exception {
};

namespace Detail {

template <typename T>
struct TaskResult final {
    T value;
};

template <>
struct TaskResult<void> final {};

struct TaskImpl;

enum class TaskStatus : std::int8_t {
    Created,
    RanToCompletion,
    Rejected,
};

template <typename TResult>
class POMDOG_EXPORT TaskBody final
    : public std::enable_shared_from_this<TaskBody<TResult>> {
public:
    TaskResult<TResult> result;
    std::exception_ptr exceptionPointer;
    std::vector<std::function<void()>> continuations;
    std::mutex mutex;
    std::atomic<TaskStatus> status;

    friend class TaskCompletionSource<TResult>;

public:
    TaskBody(const TaskBody&) = delete;
    TaskBody & operator=(const TaskBody&) = delete;

    TaskBody()
        : status(TaskStatus::Created)
    {
    }

    bool IsDone() const noexcept
    {
        const auto s = status.load();
        return (s == TaskStatus::RanToCompletion) || (s == TaskStatus::Rejected);
    }

    void SetResult(TaskResult<TResult> && resultIn)
    {
        POMDOG_ASSERT(!this->IsDone());
        status.store(TaskStatus::RanToCompletion);

        std::vector<std::function<void()>> swapped;
        {
            std::lock_guard<std::mutex> lock(mutex);
            result = std::forward<TaskResult<TResult>>(resultIn);
            std::swap(continuations, swapped);
        }
        for (auto & continuation : swapped) {
            continuation();
        }
    }

    void SetException(const std::exception_ptr& exception)
    {
        POMDOG_ASSERT(!this->IsDone());
        status.store(TaskStatus::Rejected);

        std::vector<std::function<void()>> swapped;
        {
            std::lock_guard<std::mutex> lock(mutex);
            exceptionPointer = exception;
            std::swap(continuations, swapped);
        }
        for (auto & continuation : swapped) {
            continuation();
        }
    }
};

namespace TypeTraitsImpl {

#if __cplusplus <= 201402L
template <class...>
struct MakeVoid {
    using type = void;
};

template <class... T>
using void_t = typename MakeVoid<T...>::type;
#else
template <class... T>
using void_t = typename std::void_t<T...>;
#endif

template <class F, class = void_t<>>
struct ResultOf {};

template <class Functor>
struct ResultOf<Functor, void_t<decltype(&Functor::operator())>>
    : ResultOf<decltype(&Functor::operator())> {
};

template <class Result, class T, class... Args>
struct ResultOf<Result(T::*)(Args...)> {
    using Type = Result;
};

template <class Result, class T, class... Args>
struct ResultOf<Result(T::*)(Args...) const> {
    using Type = Result;
};

template <class Result, class... Args>
struct ResultOf<Result(*)(Args...)> {
    using Type = Result;
};

template <class F, class = void_t<>>
struct ArgOf {};

template <class Functor>
struct ArgOf<Functor, void_t<decltype(&Functor::operator())>>
    : ArgOf<decltype(&Functor::operator())> {
};

template <class Result, class T, class Arg>
struct ArgOf<Result(T::*)(Arg)> {
    using Type = Arg;
};

template <class Result, class T, class Arg>
struct ArgOf<Result(T::*)(Arg) const> {
    using Type = Arg;
};

template <class Result, class Arg>
struct ArgOf<Result(*)(Arg)> {
    using Type = Arg;
};

} // namespace TypeTraitsImpl

template <typename Function>
using ResultOf = typename TypeTraitsImpl::ResultOf<Function>::Type;

template <typename Function>
using ArgumentOf = std::remove_const_t<std::remove_reference_t<typename TypeTraitsImpl::ArgOf<Function>::Type>>;

template <typename T, typename U>
using IsSame = typename std::enable_if<
    std::is_same<T, U>::value, std::nullptr_t>::type;

template <typename T>
struct TaskTypeTraits {
    typedef T ResultType;
};

template <typename T>
struct TaskTypeTraits<Task<T>> {
    typedef T ResultType;
};

template <typename TFunction>
using TaskResultOf = typename TaskTypeTraits<ResultOf<TFunction>>::ResultType;

} // namespace Detail

template <typename TResult>
class POMDOG_EXPORT TaskCompletionSource final {
private:
    template <typename T> friend class Task;
    std::shared_ptr<Detail::TaskBody<TResult>> body;

public:
    TaskCompletionSource()
        : body(std::make_shared<Detail::TaskBody<TResult>>())
    {}

    void SetResult(const TResult& result) const
    {
        POMDOG_ASSERT(body);
        Detail::TaskResult<TResult> wrapper;
        wrapper.value = result;
        body->SetResult(std::move(wrapper));
    }

    void SetException(const std::exception_ptr& exception) const
    {
        POMDOG_ASSERT(body);
        body->SetException(exception);
    }
};

template <>
class POMDOG_EXPORT TaskCompletionSource<void> final {
private:
    template <typename T> friend class Task;
    std::shared_ptr<Detail::TaskBody<void>> body;

public:
    TaskCompletionSource()
        : body(std::make_shared<Detail::TaskBody<void>>())
    {}

    void SetResult() const
    {
        POMDOG_ASSERT(body);
        Detail::TaskResult<void> wrapper;
        body->SetResult(std::move(wrapper));
    }

    void SetException(const std::exception_ptr& exception) const
    {
        POMDOG_ASSERT(body);
        body->SetException(exception);
    }
};

template <typename TResult>
class POMDOG_EXPORT Task final {
private:
    friend struct Detail::TaskImpl;

    std::shared_ptr<Detail::TaskBody<TResult>> body;

public:
    Task()
        : body(std::make_shared<Detail::TaskBody<TResult>>())
    {}

    explicit Task(const TaskCompletionSource<TResult>& tcs)
        : body(tcs.body)
    {
        POMDOG_ASSERT(body);
    }

    template <typename TFunction>
    auto Then(const TFunction& continuation) const
        -> Task<Detail::TaskResultOf<TFunction>>;

    template <typename TFunction>
    Task<void> Catch(const TFunction& func) const;

    template <typename TFunction>
    auto ContinueWith(const TFunction& continuation) const
        -> Task<Detail::TaskResultOf<TFunction>>;

    bool IsDone() const
    {
        POMDOG_ASSERT(body);
        return body->IsDone();
    }

    bool IsRejected() const
    {
        POMDOG_ASSERT(body);
        return body->status.load() == Detail::TaskStatus::Rejected;
    }
};

namespace Detail {

template <typename TFunction, typename TResult>
POMDOG_EXPORT
auto InnerGetTask(
    const TFunction& continuation,
    const TaskResult<TResult>& result) -> decltype(continuation(result.value))
{
    return continuation(result.value);
}

template <typename TFunction>
POMDOG_EXPORT
auto InnerGetTask(
    const TFunction& continuation,
    const TaskResult<void>&) -> decltype(continuation())
{
    return continuation();
}

struct POMDOG_EXPORT TaskImpl {
    template <typename T, typename Func>
    static void ScheduleContinuation(const Task<T>& task, Func && continuation)
    {
        if (task.IsDone()) {
            continuation();
        }
        else {
            std::lock_guard<std::mutex> lock(task.body->mutex);
            task.body->continuations.push_back(std::move(continuation));
        }
    }

    template <typename TResult>
    static void InnerSetResult(
        const TaskCompletionSource<TResult>& tcs,
        const TaskResult<TResult>& result)
    {
        tcs.SetResult(result.value);
    }

    static void InnerSetResult(
        const TaskCompletionSource<void>& tcs,
        const TaskResult<void>&)
    {
        tcs.SetResult();
    }

    template <typename TFunction, typename TResult>
    static void InnerInvoke(
        const TFunction& func,
        const TaskCompletionSource<TResult>& tcs,
        IsSame<ResultOf<TFunction>, TResult> = nullptr)
    {
        tcs.SetResult(func());
    }

    template <typename TFunction>
    static void InnerInvoke(
        const TFunction& func,
        const TaskCompletionSource<void>& tcs,
        IsSame<ResultOf<TFunction>, void> = nullptr)
    {
        func();
        tcs.SetResult();
    }

    template <typename TFunction, typename TResult>
    static void InnerInvoke(
        const TFunction& func,
        const TaskCompletionSource<TResult>& tcs,
        IsSame<ResultOf<TFunction>, Task<TResult>> = nullptr)
    {
        auto task = func();

        ScheduleContinuation(task, [antecedent = task.body, tcs] {
            if (antecedent->exceptionPointer) {
                tcs.SetException(antecedent->exceptionPointer);
                return;
            }
            InnerSetResult(tcs, antecedent->result);
        });
    }

    template <typename TFunction, typename TResult, typename TContinuationResult>
    static void InnerInvokeContinuation(
        const TFunction& continuation,
        const TaskCompletionSource<TContinuationResult>& tcs,
        const TaskResult<TResult>& result,
        IsSame<ResultOf<TFunction>, TContinuationResult> = nullptr)
    {
        tcs.SetResult(continuation(result.value));
    }

    template <typename TFunction, typename TResult>
    static void InnerInvokeContinuation(
        const TFunction& continuation,
        const TaskCompletionSource<void>& tcs,
        const TaskResult<TResult>& result,
        IsSame<ResultOf<TFunction>, void> = nullptr)
    {
        continuation(result.value);
        tcs.SetResult();
    }

    template <typename TFunction, typename TContinuationResult>
    static void InnerInvokeContinuation(
        const TFunction& continuation,
        const TaskCompletionSource<TContinuationResult>& tcs,
        const TaskResult<void>&,
        IsSame<ResultOf<TFunction>, TContinuationResult> = nullptr)
    {
        tcs.SetResult(continuation());
    }

    template <typename TFunction>
    static void InnerInvokeContinuation(
        const TFunction& continuation,
        const TaskCompletionSource<void>& tcs,
        const TaskResult<void>&,
        IsSame<ResultOf<TFunction>, void> = nullptr)
    {
        continuation();
        tcs.SetResult();
    }

    template <typename TFunction, typename TResult, typename TContinuationResult>
    static void InnerInvokeContinuation(
        const TFunction& continuation,
        const TaskCompletionSource<TContinuationResult>& tcs,
        const TaskResult<TResult>& result,
        IsSame<ResultOf<TFunction>, Task<TContinuationResult>> = nullptr)
    {
        auto task = InnerGetTask(continuation, result);

        ScheduleContinuation(task, [antecedent = task.body, tcs] {
            if (antecedent->exceptionPointer) {
                tcs.SetException(antecedent->exceptionPointer);
                return;
            }
            InnerSetResult(tcs, antecedent->result);
        });
    }

    template <typename TFunction, typename TResult, typename TContinuationResult>
    static void InnerInvokeContinuationWithTask(
        const TFunction& continuation,
        const TaskCompletionSource<TContinuationResult>& tcs,
        const TaskBody<TResult>& task,
        IsSame<ResultOf<TFunction>, TContinuationResult> = nullptr)
    {
        tcs.SetResult(continuation(task));
    }

    template <typename TFunction, typename TResult>
    static void InnerInvokeContinuationWithTask(
        const TFunction& continuation,
        const TaskCompletionSource<void>& tcs,
        const Task<TResult>& task,
        IsSame<ResultOf<TFunction>, void> = nullptr)
    {
        continuation(task);
        tcs.SetResult();
    }

    template <typename T>
    static const TaskResult<T>& GetResult(const Task<T>& task)
    {
        return task.body->result;
    }

    template <typename T>
    static const std::exception_ptr& GetExceptionPointer(const Task<T>& task)
    {
        return task.body->exceptionPointer;
    }
};

template <typename TException>
struct POMDOG_EXPORT InnerHandleException final {
    template <typename Function>
    static void Perform(const std::exception_ptr& exception, const Function& onRejection)
    {
        try {
            if (exception) {
                std::rethrow_exception(exception);
            }
        }
        catch (const TException& ex) {
            onRejection(ex);
        }
        catch (...) {
        }
    }
};

template <>
struct POMDOG_EXPORT InnerHandleException<std::exception_ptr> final {
    template <typename Function>
    static void Perform(const std::exception_ptr& exception, const Function& onRejection)
    {
        if (exception) {
            onRejection(exception);
        }
    }
};

} // namespace Detail

template <typename TResult>
template <typename TFunction>
auto Task<TResult>::Then(const TFunction& continuation) const
    -> Task<Detail::TaskResultOf<TFunction>>
{
    POMDOG_ASSERT(body);

    using TContinuationResult = Detail::TaskResultOf<TFunction>;
    TaskCompletionSource<TContinuationResult> tcs;

    Detail::TaskImpl::ScheduleContinuation(*this,
        [antecedent = body, continuation, tcs] {
            POMDOG_ASSERT(antecedent);
            POMDOG_ASSERT(antecedent->IsDone());
            if (antecedent->exceptionPointer) {
                tcs.SetException(antecedent->exceptionPointer);
                return;
            }
            try {
                Detail::TaskImpl::InnerInvokeContinuation(
                    continuation, tcs, antecedent->result);
            }
            catch (...) {
                tcs.SetException(std::current_exception());
            }
        });

    Task<TContinuationResult> task(std::move(tcs));
    return task;
}

template <typename TResult>
template <typename TFunction>
Task<void> Task<TResult>::Catch(const TFunction& func) const
{
    POMDOG_ASSERT(body);

    using TException = Detail::ArgumentOf<TFunction>;
    TaskCompletionSource<void> tcs;

    Detail::TaskImpl::ScheduleContinuation(*this,
        [antecedent = body, onRejection = func, tcs] {
            POMDOG_ASSERT(antecedent);
            POMDOG_ASSERT(antecedent->IsDone());
            Detail::InnerHandleException<TException>::Perform(
                antecedent->exceptionPointer, onRejection);
            tcs.SetResult();
        });

    Task<void> task(std::move(tcs));
    return task;
}

template <typename TResult>
template <typename TFunction>
auto Task<TResult>::ContinueWith(const TFunction& continuation) const
    -> Task<Detail::TaskResultOf<TFunction>>
{
    POMDOG_ASSERT(body);

    using TContinuationResult = Detail::TaskResultOf<TFunction>;
    TaskCompletionSource<TContinuationResult> tcs;

    Detail::TaskImpl::ScheduleContinuation(*this,
        [antecedent = *this, continuation, tcs] {
            POMDOG_ASSERT(antecedent.body);
            POMDOG_ASSERT(antecedent.body->IsDone());
            try {
                Detail::TaskImpl::InnerInvokeContinuationWithTask(
                    continuation, tcs, antecedent);
            }
            catch (...) {
                tcs.SetException(std::current_exception());
            }
        });

    Task<TContinuationResult> task(std::move(tcs));
    return task;
}

template <typename TResult, typename TFunction>
POMDOG_EXPORT
auto CreateTask(const TFunction& func) -> Task<TResult>
{
    TaskCompletionSource<TResult> tcs;
    func(tcs);
    Task<TResult> task(std::move(tcs));
    return task;
}

template <typename TResult>
POMDOG_EXPORT
Task<TResult> FromResult(TResult && result)
{
    static_assert(!std::is_reference<TResult>::value, "");
    TaskCompletionSource<TResult> tcs;
    tcs.SetResult(std::forward<TResult>(result));
    Task<TResult> task(std::move(tcs));
    return task;
}

namespace Detail {

struct WhenAnyPromise final {
    std::atomic_bool isAnyTaskComplete;
};

template <typename TResult>
struct WhenAllPromise final {
    std::mutex mutex;
    int count;
    bool isRejected;
    std::vector<TResult> results;
};

template <>
struct WhenAllPromise<void> final {
    std::mutex mutex;
    int count;
    bool isRejected;
};

template <typename TResult>
struct POMDOG_EXPORT TaskFromDefaultResult final {
    static Task<TResult> Perform()
    {
        return FromResult<TResult>({});
    }
};

template <>
struct POMDOG_EXPORT TaskFromDefaultResult<void> final {
    static Task<void> Perform()
    {
        TaskCompletionSource<void> tcs;
        tcs.SetResult();
        Task<void> task(std::move(tcs));
        return task;
    }
};

template <typename TResult>
POMDOG_EXPORT
Task<std::vector<TResult>> WhenAllImpl(const std::vector<Task<TResult>>& tasks)
{
    if (tasks.empty()) {
        return FromResult<std::vector<TResult>>({});
    }

    POMDOG_ASSERT(!tasks.empty());

    TaskCompletionSource<std::vector<TResult>> tcs;
    auto whenAllPromise = std::make_shared<Detail::WhenAllPromise<TResult>>();
    whenAllPromise->count = static_cast<int>(tasks.size());
    whenAllPromise->isRejected = false;

    for (auto & task : tasks) {
        task.ContinueWith([tcs, whenAllPromise](const Task<TResult>& t) {
            std::lock_guard<std::mutex> lock(whenAllPromise->mutex);
            if (whenAllPromise->isRejected) {
                return;
            }
            if (t.IsRejected()) {
                whenAllPromise->isRejected = true;
                tcs.SetException(TaskImpl::GetExceptionPointer(t));
            }
            else {
                const auto& result = TaskImpl::GetResult(t).value;
                whenAllPromise->results.push_back(result);
                --whenAllPromise->count;
                POMDOG_ASSERT(whenAllPromise->count >= 0);
                if (whenAllPromise->count <= 0) {
                    tcs.SetResult(std::move(whenAllPromise->results));
                }
            }
        });
    }
    Task<std::vector<TResult>> task(std::move(tcs));
    return task;
}

POMDOG_EXPORT
Task<void> WhenAllImpl(const std::vector<Task<void>>& tasks);

} // namespace Detail

template <typename TResult>
POMDOG_EXPORT
Task<TResult> WhenAny(const std::vector<Task<TResult>>& tasks)
{
    if (tasks.empty()) {
        return Detail::TaskFromDefaultResult<TResult>::Perform();
    }

    POMDOG_ASSERT(!tasks.empty());

    TaskCompletionSource<TResult> tcs;
    auto whenAnyPromise = std::make_shared<Detail::WhenAnyPromise>();
    whenAnyPromise->isAnyTaskComplete.store(false);

    for (auto & task : tasks) {
        task.ContinueWith([tcs, whenAnyPromise](const Task<TResult>& t) {
            if (whenAnyPromise->isAnyTaskComplete.exchange(true)) {
                return;
            }
            if (t.IsRejected()) {
                tcs.SetException(Detail::TaskImpl::GetExceptionPointer(t));
            }
            else {
                Detail::TaskImpl::InnerSetResult(tcs, Detail::TaskImpl::GetResult(t));
            }
        });
    }
    Task<TResult> task(std::move(tcs));
    return task;
}

template <typename TaskType>
POMDOG_EXPORT
auto WhenAll(const std::vector<TaskType>& tasks) -> decltype(Detail::WhenAllImpl(tasks))
{
    return Detail::WhenAllImpl(tasks);
}

} // namespace Concurrency
} // namespace Pomdog
