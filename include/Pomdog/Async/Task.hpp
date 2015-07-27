// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TASK_9C530CDA_HPP
#define POMDOG_TASK_9C530CDA_HPP

#include "Pomdog/Async/Scheduler.hpp"
#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <type_traits>
#include <vector>

namespace Pomdog {
namespace Concurrency {

template <typename TResult>
class Task;

class TaskFactory;

template <typename TResult>
class TaskCompletionSource;

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
    bool ranToCompletion;

    friend class TaskCompletionSource<TResult>;
    friend class TaskFactory;
    friend struct TaskImpl;

public:
    TaskBody() = delete;
    TaskBody(TaskBody const&) = delete;
    TaskBody & operator=(TaskBody const&) = delete;

    explicit TaskBody(std::shared_ptr<Scheduler> const& scheduler);

    void RunContinuations();
};

namespace FunctionTypeTraitsImpl {

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

} // namespace FunctionTypeTraitsImpl

template <typename TFunction>
using ResultOf = decltype(FunctionTypeTraitsImpl::Invoke(
    std::declval<std::remove_pointer_t<TFunction>*>()));

template <typename TFunction>
using ArgumentOf = std::remove_const_t<std::remove_reference_t<
    decltype(FunctionTypeTraitsImpl::GetArg(
    std::declval<std::remove_pointer_t<TFunction>*>()))>>;

template <typename T, typename U, bool B>
using EnableIf = std::enable_if_t<std::is_same<T, U>::value == B, std::nullptr_t>;

template <typename TResult>
struct TaskTypeTraits {
    typedef TResult ResultType;
};
template <typename TResult>
struct TaskTypeTraits<Task<TResult>> {
    typedef TResult ResultType;
};

} // namespace Detail

template <typename TResult>
class POMDOG_EXPORT Task {
private:
    typedef Detail::TaskBody<TResult> TaskBody;
    std::shared_ptr<TaskBody> body;

    friend class TaskCompletionSource<TResult>;
    friend class TaskFactory;
    friend struct Detail::TaskImpl;

public:
    Task() = delete;

    explicit Task(std::shared_ptr<Scheduler> const& scheduler);

    explicit Task(std::shared_ptr<TaskBody> const& taskBody);

    template <typename TFunction>
    auto Then(TFunction const& continuation)
        -> Task<typename Detail::TaskTypeTraits<Detail::ResultOf<TFunction>>::ResultType>;

    template <typename TFunction>
    auto Then(
        TFunction const& continuation,
        std::shared_ptr<Scheduler> const& scheduler)
        -> Task<typename Detail::TaskTypeTraits<Detail::ResultOf<TFunction>>::ResultType>;

    template <typename TFunction>
    Task<TResult> Catch(TFunction const& func);

    template <typename TFunction>
    Task<TResult> Catch(
        TFunction const& func,
        std::shared_ptr<Scheduler> const& scheduler);

    bool IsDone() const;

    bool IsRejected() const;

    std::shared_ptr<Scheduler> GetScheduler() const noexcept;
};

class POMDOG_EXPORT TaskFactory final {
private:
    std::shared_ptr<Scheduler> ambientScheduler;

public:
    TaskFactory()
        : ambientScheduler(std::make_shared<Concurrency::ImmediateExecutor>()){}

    explicit TaskFactory(std::shared_ptr<Scheduler> scheduler)
        : ambientScheduler(scheduler){}

    std::shared_ptr<Scheduler> GetScheduler() const noexcept;

    template <typename TFunction>
    auto CreateTask(TFunction const& func) const
        -> Task<Detail::ResultOf<TFunction>>;

    template <typename TFunction>
    static auto CreateTask(TFunction const& func, std::shared_ptr<Scheduler> const& scheduler)
        -> Task<Detail::ResultOf<TFunction>>;

    Task<void> Delay(Duration const& delay) const;

    static Task<void> Delay(Duration const& delay, std::shared_ptr<Scheduler> const& scheduler);

    Task<void> FromResult() const;

    template <typename TResult>
    Task<TResult> FromResult(TResult && result) const;

    template <typename TResult>
    static Task<TResult> FromResult(TResult && result, std::shared_ptr<Scheduler> const& scheduler);

    Task<void> WhenAny(std::vector<Task<void>> && tasks) const;

    static Task<void> WhenAny(std::vector<Task<void>> && tasks, std::shared_ptr<Scheduler> const& scheduler);

//    template <typename TResult>
//    Task<std::vector<TResult>>
//    WhenAny(std::vector<Task<TResult>> && tasks);
//
//    template <typename TResult>
//    Task<std::vector<TResult>>
//    WhenAny(std::vector<Task<TResult>> && tasks, std::shared_ptr<Scheduler> const& scheduler);

    Task<void> WhenAll(std::vector<Task<void>> && tasks) const;

    static Task<void> WhenAll(std::vector<Task<void>> && tasks, std::shared_ptr<Scheduler> const& scheduler);

//    template <typename TResult>
//    Task<std::vector<TResult>>
//    WhenAll(std::vector<Task<TResult>> && tasks);
//
//    template <typename TResult>
//    Task<std::vector<TResult>>
//    WhenAll(std::vector<Task<TResult>> && tasks, std::shared_ptr<Scheduler> const& scheduler);
};

template <typename TResult>
class POMDOG_EXPORT TaskCompletionSource {
private:
    Task<TResult> task;

public:
    explicit TaskCompletionSource(std::shared_ptr<Scheduler> const& scheduler);

    Task<TResult> GetTask() const;

    void SetResult(TResult const& result);

    void SetException(std::exception_ptr const& exception);
};

template <>
class POMDOG_EXPORT TaskCompletionSource<void> {
private:
    Task<void> task;

public:
    explicit TaskCompletionSource(Task<void> const& task);

    explicit TaskCompletionSource(std::shared_ptr<Scheduler> const& scheduler);

    Task<void> GetTask() const;

    void SetResult();

    void SetException(std::exception_ptr const& exception);
};

namespace Detail {

template <typename TFunction, typename TResult>
void POMDOG_EXPORT InnerInvoke(
    TFunction const& func,
    std::shared_ptr<TaskCompletionSource<TResult>> const& tcs)
{
    tcs->SetResult(func());
}

template <typename TFunction>
void POMDOG_EXPORT InnerInvoke(
    TFunction const& func,
    std::shared_ptr<TaskCompletionSource<void>> const& tcs)
{
    func();
    tcs->SetResult();
}

template <typename TFunction, typename TResult>
auto POMDOG_EXPORT InnerInvokeGetTask(
    TFunction const& continuation,
    Detail::TaskResult<TResult> const& result) -> ResultOf<TFunction>
{
    return continuation(result.value);
}

template <typename TFunction>
auto POMDOG_EXPORT InnerInvokeGetTask(
    TFunction const& continuation,
    Detail::TaskResult<void> const&) -> ResultOf<TFunction>
{
    return continuation();
}

struct TaskImpl {
    template <typename TResult>
    static void InnerInvokeSetResult(
        TaskCompletionSource<TResult> & tcs,
        Detail::TaskResult<TResult> const& result)
    {
        tcs.SetResult(result.value);
    }

    static void InnerInvokeSetResult(
        TaskCompletionSource<void> & tcs,
        Detail::TaskResult<void> const&)
    {
        tcs.SetResult();
    }

    template <typename TFunction, typename TResult, typename TContinuationResult>
    static void POMDOG_EXPORT InnerInvokeContinuation(
        TFunction const& continuation,
        std::shared_ptr<TaskCompletionSource<TContinuationResult>> const& tcs,
        Detail::TaskResult<TResult> const& result,
        EnableIf<ResultOf<TFunction>, Task<TContinuationResult>, false> = nullptr)
    {
        tcs->SetResult(continuation(result.value));
    }

    template <typename TFunction, typename TResult>
    static void POMDOG_EXPORT InnerInvokeContinuation(
        TFunction const& continuation,
        std::shared_ptr<TaskCompletionSource<void>> const& tcs,
        Detail::TaskResult<TResult> const& result,
        EnableIf<ResultOf<TFunction>, Task<void>, false> = nullptr)
    {
        continuation(result.value);
        tcs->SetResult();
    }

    template <typename TFunction, typename TContinuationResult>
    static void POMDOG_EXPORT InnerInvokeContinuation(
        TFunction const& continuation,
        std::shared_ptr<TaskCompletionSource<TContinuationResult>> const& tcs,
        Detail::TaskResult<void> const&,
        EnableIf<ResultOf<TFunction>, Task<TContinuationResult>, false> = nullptr)
    {
        tcs->SetResult(continuation());
    }

    template <typename TFunction>
    static void POMDOG_EXPORT InnerInvokeContinuation(
        TFunction const& continuation,
        std::shared_ptr<TaskCompletionSource<void>> const& tcs,
        Detail::TaskResult<void> const&,
        EnableIf<ResultOf<TFunction>, Task<void>, false> = nullptr)
    {
        continuation();
        tcs->SetResult();
    }

    template <typename TFunction, typename TResult, typename TContinuationResult>
    static void POMDOG_EXPORT InnerInvokeContinuation(
        TFunction const& continuation,
        std::shared_ptr<TaskCompletionSource<TContinuationResult>> const& tcs,
        Detail::TaskResult<TResult> const& result,
        EnableIf<ResultOf<TFunction>, Task<TContinuationResult>, true> = nullptr)
    {
        auto task = InnerInvokeGetTask(continuation, result);
        auto scheduler = task.GetScheduler();
        auto runContinuation = [antecedent = task.body, tcs] {
            if (antecedent->exceptionPointer) {
                tcs->SetException(antecedent->exceptionPointer);
                return;
            }
            InnerInvokeSetResult(*tcs, antecedent->result);
        };

        const bool completed = task.IsDone();
        if (completed) {
            POMDOG_ASSERT(scheduler);
            scheduler->Schedule(std::move(runContinuation));
        }
        else {
            std::lock_guard<std::mutex> lock(task.body->mutex);
            task.body->continuations.push_back(std::move(runContinuation));
        }
    }
};

template <typename TException>
struct POMDOG_EXPORT InnerHandleException {
    template <typename Function>
    void operator()(std::exception_ptr const& exception, Function const& onRejection)
    {
        try {
            if (exception) {
                std::rethrow_exception(exception);
            }
        }
        catch (TException const& ex) {
            onRejection(ex);
        }
        catch (...) {
        }
    }
};

template <>
struct POMDOG_EXPORT InnerHandleException<std::exception_ptr> {
    template <typename Function>
    void operator()(std::exception_ptr const& exception, Function const& onRejection)
    {
        if (exception) {
            onRejection(exception);
        }
    }
};

} // namespace Detail
//-----------------------------------------------------------------------
template <typename TFunction>
auto TaskFactory::CreateTask(TFunction const& func) const
    -> Task<Detail::ResultOf<TFunction>>
{
    POMDOG_ASSERT(ambientScheduler);
    return CreateTask(func, ambientScheduler);
}
//-----------------------------------------------------------------------
template <typename TFunction>
auto TaskFactory::CreateTask(
    TFunction const& func,
    std::shared_ptr<Scheduler> const& scheduler)
    -> Task<Detail::ResultOf<TFunction>>
{
    POMDOG_ASSERT(scheduler);
    using TResult = Detail::ResultOf<TFunction>;
    auto tcs = std::make_shared<TaskCompletionSource<TResult>>(scheduler);

    scheduler->Schedule([=] {
        try {
            Detail::InnerInvoke(func, tcs);
        }
        catch (...) {
            tcs->SetException(std::current_exception());
        }
    });
    return tcs->GetTask();
}
//-----------------------------------------------------------------------
template <typename TResult>
Task<TResult> TaskFactory::FromResult(TResult && result) const
{
    POMDOG_ASSERT(ambientScheduler);
    return FromResult(std::forward<TResult>(result), ambientScheduler);
}
//-----------------------------------------------------------------------
template <typename TResult>
Task<TResult> TaskFactory::FromResult(
    TResult && result, std::shared_ptr<Scheduler> const& scheduler)
{
    POMDOG_ASSERT(scheduler);
    auto tcs = std::make_shared<TaskCompletionSource<TResult>>(scheduler);
    tcs->SetResult(std::forward<TResult>(result));
    return tcs->GetTask();
}
//-----------------------------------------------------------------------
template <typename TResult>
Detail::TaskBody<TResult>::TaskBody(std::shared_ptr<Scheduler> const& schedulerIn)
    : scheduler(schedulerIn)
    , ranToCompletion(false)
{
    POMDOG_ASSERT(!scheduler.expired());
}
//-----------------------------------------------------------------------
template <typename TResult>
Task<TResult>::Task(std::shared_ptr<Scheduler> const& schedulerIn)
    : body(std::make_shared<TaskBody>(schedulerIn))
{
    POMDOG_ASSERT(body);
}
//-----------------------------------------------------------------------
template <typename TResult>
Task<TResult>::Task(std::shared_ptr<TaskBody> const& taskBodyIn)
    : body(taskBodyIn)
{
    POMDOG_ASSERT(body);
}
//-----------------------------------------------------------------------
template <typename TResult>
template <typename TFunction>
auto Task<TResult>::Then(TFunction const& continuation)
    -> Task<typename Detail::TaskTypeTraits<Detail::ResultOf<TFunction>>::ResultType>
{
    POMDOG_ASSERT(body);
    auto scheduler = body->scheduler.lock();
    POMDOG_ASSERT(scheduler);
    return Then(continuation, scheduler);
}
//-----------------------------------------------------------------------
template <typename TResult>
template <typename TFunction>
auto Task<TResult>::Then(
    TFunction const& continuation,
    std::shared_ptr<Scheduler> const& scheduler)
    -> Task<typename Detail::TaskTypeTraits<Detail::ResultOf<TFunction>>::ResultType>
{
    POMDOG_ASSERT(scheduler);
    POMDOG_ASSERT(body);

    using TContinuationResult = typename Detail::TaskTypeTraits<Detail::ResultOf<TFunction>>::ResultType;
    auto tcs = std::make_shared<TaskCompletionSource<TContinuationResult>>(scheduler);

    auto runContinuation = [antecedent = body, continuation, tcs] {
        POMDOG_ASSERT(antecedent);
        POMDOG_ASSERT(antecedent->ranToCompletion);
        if (antecedent->exceptionPointer) {
            tcs->SetException(antecedent->exceptionPointer);
            return;
        }
        try {
            Detail::TaskImpl::InnerInvokeContinuation(continuation, tcs, antecedent->result);
        }
        catch (...) {
            tcs->SetException(std::current_exception());
        }
    };

    const bool completed = this->IsDone();
    if (completed) {
        scheduler->Schedule(std::move(runContinuation));
    } else {
        std::lock_guard<std::mutex> lock(body->mutex);
        body->continuations.push_back(std::move(runContinuation));
    }
    return std::move(tcs->GetTask());
}
//-----------------------------------------------------------------------
template <typename TResult>
template <typename TFunction>
Task<TResult> Task<TResult>::Catch(TFunction const& func)
{
    POMDOG_ASSERT(body);
    auto scheduler = body->scheduler.lock();
    POMDOG_ASSERT(scheduler);
    return Catch(func, scheduler);
}
//-----------------------------------------------------------------------
template <typename TResult>
template <typename TFunction>
Task<TResult> Task<TResult>::Catch(
    TFunction const& func,
    std::shared_ptr<Scheduler> const& scheduler)
{
    POMDOG_ASSERT(body);
    POMDOG_ASSERT(scheduler);

    using TException = Detail::ArgumentOf<TFunction>;
    auto tcs = std::make_shared<TaskCompletionSource<void>>(scheduler);

    auto runErrorHandling = [antecedent = body, onRejection = func, tcs] {
        POMDOG_ASSERT(antecedent->ranToCompletion);
        Detail::InnerHandleException<TException>{}(
            antecedent->exceptionPointer, onRejection);
        tcs->SetResult();
    };

    const bool completed = this->IsDone();
    if (completed) {
        scheduler->Schedule(runErrorHandling);
    } else {
        std::lock_guard<std::mutex> lock(body->mutex);
        body->continuations.push_back(runErrorHandling);
    }
    return std::move(tcs->GetTask());
}
//-----------------------------------------------------------------------
template <typename TResult>
void Detail::TaskBody<TResult>::RunContinuations()
{
    POMDOG_ASSERT(!scheduler.expired());
    auto sharedScheduler = scheduler.lock();
    POMDOG_ASSERT(sharedScheduler);

    for (auto & continuation : continuations) {
        sharedScheduler->Schedule(std::move(continuation));
    }
    continuations.clear();
}
//-----------------------------------------------------------------------
template <typename TResult>
bool Task<TResult>::IsDone() const
{
    POMDOG_ASSERT(body);
    std::lock_guard<std::mutex> lock(body->mutex);
    return body->ranToCompletion;
}
//-----------------------------------------------------------------------
template <typename TResult>
bool Task<TResult>::IsRejected() const
{
    POMDOG_ASSERT(body);
    std::lock_guard<std::mutex> lock(body->mutex);
    return body->exceptionPointer.operator bool();
}
//-----------------------------------------------------------------------
template <typename TResult>
std::shared_ptr<Scheduler> Task<TResult>::GetScheduler() const noexcept
{
    POMDOG_ASSERT(body);
    std::lock_guard<std::mutex> lock(body->mutex);
    return body->scheduler.lock();
}
//-----------------------------------------------------------------------
template <typename TResult>
TaskCompletionSource<TResult>::TaskCompletionSource(std::shared_ptr<Scheduler> const& scheduler)
    : task(scheduler)
{
}
//-----------------------------------------------------------------------
template <typename TResult>
Task<TResult> TaskCompletionSource<TResult>::GetTask() const
{
    return task;
}
//-----------------------------------------------------------------------
template <typename TResult>
void TaskCompletionSource<TResult>::SetResult(TResult const& result)
{
    POMDOG_ASSERT(task.body);
    std::lock_guard<std::mutex> lock(task.body->mutex);
    POMDOG_ASSERT(!task.body->ranToCompletion);
    if (task.body->ranToCompletion) {
        throw std::runtime_error("future error");
    }
    task.body->ranToCompletion = true;
    task.body->result.value = result;
    task.body->RunContinuations();
}
//-----------------------------------------------------------------------
template <typename TResult>
void TaskCompletionSource<TResult>::SetException(std::exception_ptr const& exception)
{
    POMDOG_ASSERT(task.body);
    std::lock_guard<std::mutex> lock(task.body->mutex);
    POMDOG_ASSERT(!task.body->ranToCompletion);
    if (task.body->ranToCompletion) {
        throw std::runtime_error("future error");
    }
    task.body->ranToCompletion = true;
    task.body->exceptionPointer = exception;
    task.body->RunContinuations();
}

} // namespace Concurrency
} // namespace Pomdog

#endif // POMDOG_TASK_9C530CDA_HPP
