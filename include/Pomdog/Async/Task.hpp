// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Async/Scheduler.hpp"
#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <condition_variable>
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

class ConcurrencyException : public std::exception {
};

template <typename TResult>
POMDOG_EXPORT
auto CreateTask(const TaskCompletionSource<TResult>& tcs) -> Task<TResult>;

namespace Detail {

template <typename TResult>
struct TaskResult {
    TResult value;
};

template <>
struct TaskResult<void> {};

struct TaskImpl;

template <typename TResult>
class POMDOG_EXPORT TaskBody final
    : public std::enable_shared_from_this<TaskBody<TResult>> {
public:
    std::weak_ptr<Scheduler> scheduler;
    Detail::TaskResult<TResult> result;
    std::exception_ptr exceptionPointer;
    std::vector<std::function<void()>> continuations;
    std::mutex mutex;
    std::condition_variable condition;
    bool ranToCompletion;

    friend class TaskCompletionSource<TResult>;

public:
    TaskBody() = delete;
    TaskBody(const TaskBody&) = delete;
    TaskBody & operator=(const TaskBody&) = delete;

    explicit TaskBody(const std::shared_ptr<Scheduler>& schedulerIn)
        : scheduler(schedulerIn)
        , ranToCompletion(false)
    {
        POMDOG_ASSERT(!scheduler.expired());
    }

    void RunContinuations()
    {
        POMDOG_ASSERT(!scheduler.expired());
        auto sharedScheduler = scheduler.lock();
        POMDOG_ASSERT(sharedScheduler);

        for (auto & continuation : continuations) {
            sharedScheduler->Schedule(std::move(continuation));
        }
        continuations.clear();
    }
};

namespace TypeTraitsImpl {

template <typename Result, typename...Args>
Result Invoke(Result(*)(Args...));

template <class T, typename Result, typename...Args>
Result Invoke(Result(T::*)(Args...));

template <class T, typename Result, typename...Args>
Result Invoke(Result(T::*)(Args...) const);

template <typename T, typename Function = decltype(&T::operator())>
decltype(Invoke(std::declval<Function>())) Invoke(T*);

template <typename Result, typename Arg>
Arg GetArg(Result(*)(Arg));

template <class T, typename Result, typename Arg>
Arg GetArg(Result(T::*)(Arg));

template <class T, typename Result, typename Arg>
Arg GetArg(Result(T::*)(Arg) const);

template <typename T, typename Function = decltype(&T::operator())>
decltype(GetArg(std::declval<Function>())) GetArg(T*);

} // namespace TypeTraitsImpl

template <typename TFunction>
using ResultOf = decltype(TypeTraitsImpl::Invoke(
    std::declval<std::remove_pointer_t<TFunction>*>()));

template <typename TFunction>
using ArgumentOf = std::remove_const_t<std::remove_reference_t<
    decltype(TypeTraitsImpl::GetArg(
    std::declval<std::remove_pointer_t<TFunction>*>()))>>;

template <typename T, typename U>
using IsSame = typename std::enable_if<
    std::is_same<T, U>::value, std::nullptr_t>::type;

template <typename T>
using IsTaskResult = typename std::enable_if<
    !std::is_convertible<T, std::shared_ptr<Scheduler>>::value,
    std::nullptr_t>::type;

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

template <typename TResult, typename T>
void InnerSetTaskResult(TaskResult<TResult> & result, T && value)
{
    result.value = std::forward<T>(value);
}

inline void InnerSetTaskResult(TaskResult<void> &) {}

} // namespace Detail

POMDOG_EXPORT
std::shared_ptr<Scheduler> GetAmbientScheduler();

POMDOG_EXPORT
void SetAmbientScheduler(const std::shared_ptr<Scheduler>& scheduler);

template <typename TResult>
class POMDOG_EXPORT TaskCompletionSource final {
private:
    template <typename T> friend class Task;
    std::shared_ptr<Detail::TaskBody<TResult>> body;

public:
    TaskCompletionSource()
        : TaskCompletionSource(GetAmbientScheduler())
    {}

    explicit TaskCompletionSource(const std::shared_ptr<Scheduler>& scheduler)
        : body(std::make_shared<Detail::TaskBody<TResult>>(scheduler))
    {}

    bool SetResult(const TResult& result) const
    {
        POMDOG_ASSERT(this->body);
        std::lock_guard<std::mutex> lock(body->mutex);
        if (body->ranToCompletion) {
            return false;
        }
        body->ranToCompletion = true;
        InnerSetTaskResult(body->result, result);
        body->RunContinuations();
        body->condition.notify_all();
        return true;
    }

    bool SetException(const std::exception_ptr& exception) const
    {
        POMDOG_ASSERT(this->body);
        std::lock_guard<std::mutex> lock(body->mutex);
        if (body->ranToCompletion) {
            return false;
        }
        body->ranToCompletion = true;
        body->exceptionPointer = exception;
        body->RunContinuations();
        body->condition.notify_all();
        return true;
    }
};

template <>
class POMDOG_EXPORT TaskCompletionSource<void> final {
private:
    template <typename T> friend class Task;
    std::shared_ptr<Detail::TaskBody<void>> body;

public:
    TaskCompletionSource()
        : TaskCompletionSource(GetAmbientScheduler())
    {}

    explicit TaskCompletionSource(const std::shared_ptr<Scheduler>& scheduler)
        : body(std::make_shared<Detail::TaskBody<void>>(scheduler))
    {}

    bool SetResult() const
    {
        POMDOG_ASSERT(this->body);
        std::lock_guard<std::mutex> lock(body->mutex);
        if (body->ranToCompletion) {
            return false;
        }
        body->ranToCompletion = true;
        body->RunContinuations();
        body->condition.notify_all();
        return true;
    }

    bool SetException(const std::exception_ptr& exception) const
    {
        POMDOG_ASSERT(this->body);
        std::lock_guard<std::mutex> lock(body->mutex);
        if (body->ranToCompletion) {
            return false;
        }
        body->ranToCompletion = true;
        body->exceptionPointer = exception;
        body->RunContinuations();
        body->condition.notify_all();
        return true;
    }
};

template <typename TResult>
class POMDOG_EXPORT Task {
private:
    friend struct Detail::TaskImpl;

    std::shared_ptr<Detail::TaskBody<TResult>> body;

public:
    Task() : Task(GetAmbientScheduler()) {}

    explicit Task(const std::shared_ptr<Scheduler>& scheduler)
        : body(std::make_shared<Detail::TaskBody<TResult>>(scheduler))
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
    auto Then(
        const TFunction& continuation,
        const std::shared_ptr<Scheduler>& scheduler) const
        -> Task<Detail::TaskResultOf<TFunction>>;

    template <typename TFunction>
    Task<void> Catch(const TFunction& func) const;

    template <typename TFunction>
    Task<void> Catch(
        const TFunction& func,
        const std::shared_ptr<Scheduler>& scheduler) const;

    template <typename TFunction>
    auto ContinueWith(const TFunction& continuation) const
        -> Task<Detail::TaskResultOf<TFunction>>;

    template <typename TFunction>
    auto ContinueWith(
        const TFunction& continuation,
        const std::shared_ptr<Scheduler>& scheduler) const
        -> Task<Detail::TaskResultOf<TFunction>>;

    void Wait() const
    {
        POMDOG_ASSERT(body);
        std::unique_lock<std::mutex> lock(body->mutex);
        if (body->ranToCompletion) {
            return;
        }
        body->condition.wait(lock, [this]{ return body->ranToCompletion; });
    }

    bool IsDone() const
    {
        POMDOG_ASSERT(body);
        std::lock_guard<std::mutex> lock(body->mutex);
        return body->ranToCompletion;
    }

    bool IsRejected() const
    {
        POMDOG_ASSERT(body);
        std::lock_guard<std::mutex> lock(body->mutex);
        return body->exceptionPointer.operator bool();
    }

    std::shared_ptr<Scheduler> GetScheduler() const noexcept
    {
        POMDOG_ASSERT(body);
        std::lock_guard<std::mutex> lock(body->mutex);
        return body->scheduler.lock();
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
    static void ScheduleContinuation(
        const Task<T>& task,
        const std::shared_ptr<Scheduler>& scheduler,
        Func && continuation)
    {
        if (task.IsDone()) {
            scheduler->Schedule(std::move(continuation));
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
        auto scheduler = task.GetScheduler();
        POMDOG_ASSERT(scheduler);

        ScheduleContinuation(task, scheduler, [antecedent = task.body, tcs] {
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
        auto scheduler = task.GetScheduler();
        POMDOG_ASSERT(scheduler);

        ScheduleContinuation(task, scheduler, [antecedent = task.body, tcs] {
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
struct POMDOG_EXPORT InnerHandleException {
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
struct POMDOG_EXPORT InnerHandleException<std::exception_ptr> {
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
    return Then(continuation, body->scheduler.lock());
}

template <typename TResult>
template <typename TFunction>
auto Task<TResult>::Then(
    const TFunction& continuation,
    const std::shared_ptr<Scheduler>& scheduler) const
    -> Task<Detail::TaskResultOf<TFunction>>
{
    POMDOG_ASSERT(body);
    POMDOG_ASSERT(scheduler);

    using TContinuationResult = Detail::TaskResultOf<TFunction>;
    TaskCompletionSource<TContinuationResult> tcs(scheduler);

    Detail::TaskImpl::ScheduleContinuation(*this, scheduler,
        [antecedent = body, continuation, tcs] {
            POMDOG_ASSERT(antecedent);
            POMDOG_ASSERT(antecedent->ranToCompletion);
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

    return CreateTask(tcs);
}

template <typename TResult>
template <typename TFunction>
Task<void> Task<TResult>::Catch(const TFunction& func) const
{
    POMDOG_ASSERT(body);
    return Catch(func, body->scheduler.lock());
}

template <typename TResult>
template <typename TFunction>
Task<void> Task<TResult>::Catch(
    const TFunction& func,
    const std::shared_ptr<Scheduler>& scheduler) const
{
    POMDOG_ASSERT(body);
    POMDOG_ASSERT(scheduler);

    using TException = Detail::ArgumentOf<TFunction>;
    TaskCompletionSource<void> tcs(scheduler);

    Detail::TaskImpl::ScheduleContinuation(*this, scheduler,
        [antecedent = body, onRejection = func, tcs] {
            POMDOG_ASSERT(antecedent->ranToCompletion);
            Detail::InnerHandleException<TException>::Perform(
                antecedent->exceptionPointer, onRejection);
            tcs.SetResult();
        });

    return CreateTask(tcs);
}

template <typename TResult>
template <typename TFunction>
auto Task<TResult>::ContinueWith(const TFunction& continuation) const
    -> Task<Detail::TaskResultOf<TFunction>>
{
    POMDOG_ASSERT(body);
    return ContinueWith(continuation, body->scheduler.lock());
}

template <typename TResult>
template <typename TFunction>
auto Task<TResult>::ContinueWith(
    const TFunction& continuation,
    const std::shared_ptr<Scheduler>& scheduler) const
    -> Task<Detail::TaskResultOf<TFunction>>
{
    POMDOG_ASSERT(body);
    POMDOG_ASSERT(scheduler);

    using TContinuationResult = Detail::TaskResultOf<TFunction>;
    TaskCompletionSource<TContinuationResult> tcs(scheduler);

    Detail::TaskImpl::ScheduleContinuation(*this, scheduler,
        [antecedent = *this, continuation, tcs] {
            POMDOG_ASSERT(antecedent.body);
            POMDOG_ASSERT(antecedent.body->ranToCompletion);
            try {
                Detail::TaskImpl::InnerInvokeContinuationWithTask(
                    continuation, tcs, antecedent);
            }
            catch (...) {
                tcs.SetException(std::current_exception());
            }
        });

    return CreateTask(tcs);
}

template <typename TResult>
POMDOG_EXPORT
auto Get(const Task<TResult>& task) -> TResult
{
    task.Wait();
    if (auto execption = Detail::TaskImpl::GetExceptionPointer(task)) {
        std::rethrow_exception(execption);
    }
    return Detail::TaskImpl::GetResult(task).value;
}

POMDOG_EXPORT
void Get(const Task<void>& task);

//-----------------------------------------------------------------------
template <typename TResult>
POMDOG_EXPORT
auto CreateTask(const TaskCompletionSource<TResult>& tcs) -> Task<TResult>
{
    Task<TResult> task(tcs);
    return std::move(task);
}
//-----------------------------------------------------------------------
template <typename TFunction>
POMDOG_EXPORT
auto CreateTask(
    const TFunction& func,
    const std::shared_ptr<Scheduler>& scheduler = GetAmbientScheduler())
    -> Task<Detail::TaskResultOf<TFunction>>
{
    POMDOG_ASSERT(scheduler);
    using TResult = Detail::TaskResultOf<TFunction>;
    TaskCompletionSource<TResult> tcs(scheduler);

    scheduler->Schedule([=] {
        try {
            Detail::TaskImpl::InnerInvoke(func, tcs);
        }
        catch (...) {
            tcs.SetException(std::current_exception());
        }
    });
    return CreateTask(tcs);
}
//-----------------------------------------------------------------------
POMDOG_EXPORT
Task<void> Delay(
    const Duration& dueTime,
    const std::shared_ptr<Scheduler>& scheduler = GetAmbientScheduler());
//-----------------------------------------------------------------------
template <typename TResult, Detail::IsTaskResult<TResult> = nullptr>
POMDOG_EXPORT
Task<TResult> FromResult(
    TResult && result,
    const std::shared_ptr<Scheduler>& scheduler = GetAmbientScheduler())
{
    static_assert(!std::is_convertible<TResult, std::shared_ptr<Scheduler>>::value, "");
    static_assert(!std::is_reference<TResult>::value, "");
    POMDOG_ASSERT(scheduler);
    TaskCompletionSource<TResult> tcs(scheduler);
    tcs.SetResult(std::forward<TResult>(result));
    return CreateTask(tcs);
}
//-----------------------------------------------------------------------
POMDOG_EXPORT
Task<void> FromResult(
    const std::shared_ptr<Scheduler>& scheduler = GetAmbientScheduler());

namespace Detail {

struct WhenAnyPromise {
    std::mutex mutex;
    bool isAnyTaskComplete;
};

template <typename TResult>
struct WhenAllPromise {
    std::mutex mutex;
    int count;
    bool isRejected;
    std::vector<TResult> results;
};

template <>
struct WhenAllPromise<void> {
    std::mutex mutex;
    int count;
    bool isRejected;
};

template <typename TResult>
struct POMDOG_EXPORT TaskFromDefaultResult {
    static Task<TResult> Perform(const std::shared_ptr<Scheduler>& scheduler)
    {
        return FromResult<TResult>({}, scheduler);
    }
};

template <>
struct POMDOG_EXPORT TaskFromDefaultResult<void> {
    static Task<void> Perform(const std::shared_ptr<Scheduler>& scheduler)
    {
        return FromResult(scheduler);
    }
};

template <typename TResult>
POMDOG_EXPORT
Task<TResult> WhenAnyImpl(
    const std::vector<Task<TResult>>& tasks,
    const std::shared_ptr<Scheduler>& scheduler)
{
    if (tasks.empty()) {
        return TaskFromDefaultResult<TResult>::Perform(scheduler);
    }

    POMDOG_ASSERT(!tasks.empty());
    POMDOG_ASSERT(scheduler);

    TaskCompletionSource<TResult> tcs(scheduler);
    auto whenAnyPromise = std::make_shared<Detail::WhenAnyPromise>();
    whenAnyPromise->isAnyTaskComplete = false;

    for (auto & task : tasks) {
        task.ContinueWith([tcs, whenAnyPromise](const Task<TResult>& t) {
            {
                std::lock_guard<std::mutex> lock(whenAnyPromise->mutex);
                if (whenAnyPromise->isAnyTaskComplete) {
                    return;
                }
                whenAnyPromise->isAnyTaskComplete = true;
            }
            if (t.IsRejected()) {
                tcs.SetException(TaskImpl::GetExceptionPointer(t));
            }
            else {
                TaskImpl::InnerSetResult(tcs, TaskImpl::GetResult(t));
            }
        });
    }
    return CreateTask(tcs);
}

template <typename TResult>
POMDOG_EXPORT
Task<std::vector<TResult>> WhenAllImpl(
    const std::vector<Task<TResult>>& tasks,
    const std::shared_ptr<Scheduler>& scheduler)
{
    if (tasks.empty()) {
        return FromResult<std::vector<TResult>>({}, scheduler);
    }

    POMDOG_ASSERT(!tasks.empty());
    POMDOG_ASSERT(scheduler);

    TaskCompletionSource<std::vector<TResult>> tcs(scheduler);
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
    return CreateTask(tcs);
}

POMDOG_EXPORT
Task<void> WhenAllImpl(
    const std::vector<Task<void>>& tasks,
    const std::shared_ptr<Scheduler>& scheduler);

} // namespace Detail

template <typename TResult>
POMDOG_EXPORT
Task<TResult> WhenAny(
    const std::vector<Task<TResult>>& tasks,
    const std::shared_ptr<Scheduler>& scheduler = GetAmbientScheduler())
{
    return Detail::WhenAnyImpl(tasks, scheduler);
}

template <typename TaskType>
POMDOG_EXPORT
auto WhenAll(
    const std::vector<TaskType>& tasks,
    const std::shared_ptr<Scheduler>& scheduler = GetAmbientScheduler())
    -> decltype(Detail::WhenAllImpl(tasks, scheduler))
{
    return Detail::WhenAllImpl(tasks, scheduler);
}

} // namespace Concurrency
} // namespace Pomdog
