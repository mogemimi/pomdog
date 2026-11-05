// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <chrono>
#include <expected>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

/// ConnectWorker runs the blocking part of connecting, name resolution and
/// the socket connection, on a thread that nothing waits for.
///
/// The owner takes the socket with `poll()` in its `update()`, which gives up
/// at the timeout even while name resolution blocks the thread. After the
/// owner gives up, or when it is destroyed, the thread closes the socket that
/// it gets, so that destroying the owner never blocks. The thread keeps what
/// it needs, such as Winsock, alive through what the connect function holds.
template <typename Socket>
class ConnectWorker final {
public:
    using Result = std::expected<Socket, std::unique_ptr<Error>>;
    using CloseFunction = void (*)(Socket) noexcept;

    ConnectWorker() noexcept = default;

    ConnectWorker(const ConnectWorker&) = delete;
    ConnectWorker& operator=(const ConnectWorker&) = delete;
    ConnectWorker(ConnectWorker&&) = delete;
    ConnectWorker& operator=(ConnectWorker&&) = delete;

    ~ConnectWorker()
    {
        abandon();
    }

    /// Starts connect, a function that returns a Result, on a new thread.
    /// closeSocket closes a socket that connect returns after the owner gave
    /// up. errorContext describes the connection in the error of a timeout.
    /// connect must not refer to the owner, which may be gone when it ends.
    template <typename Connect>
    void start(Connect&& connect, CloseFunction closeSocket, Duration timeout, std::string&& errorContext)
    {
        abandon();
        state_ = std::make_shared<State>();
        state_->closeSocket = closeSocket;
        startTime_ = Clock::now();
        timeout_ = timeout;
        errorContext_ = std::move(errorContext);

        std::thread([state = state_, connect = std::forward<Connect>(connect)]() mutable {
            auto result = connect();
            std::lock_guard<std::mutex> lock{state->mutex};
            if (state->isAbandoned) {
                if (result) {
                    state->closeSocket(*result);
                }
                return;
            }
            state->result = std::move(result);
        }).detach();
    }

    /// Returns true from `start()` until `poll()` returns the result or the
    /// owner gives up.
    [[nodiscard]] bool
    isRunning() const noexcept
    {
        return state_ != nullptr;
    }

    /// Takes the result once the connection ended, or an error once the
    /// timeout passed. Returns nullopt while the connection runs.
    [[nodiscard]] std::optional<Result>
    poll()
    {
        if (state_ == nullptr) {
            return std::nullopt;
        }

        std::optional<Result> result;
        {
            std::lock_guard<std::mutex> lock{state_->mutex};
            result = std::move(state_->result);
            state_->result.reset();
        }
        if (result.has_value()) {
            // NOTE: Release the state after the lock, since this may be its
            // last owner.
            state_ = nullptr;
            return std::optional<Result>{std::move(*result)};
        }

        // NOTE: GCC reports -Wzero-as-null-pointer-constant for comparisons of
        // durations in this template, which it rewrites through <=>, so the
        // check compares the counts.
        const auto elapsed = std::chrono::duration_cast<Duration>(Clock::now() - startTime_);
        if (elapsed.count() >= timeout_.count()) {
            abandon();
            return Result{std::unexpected(errors::wrap(errors::make("connect timed out"), std::string{errorContext_}))};
        }
        return std::nullopt;
    }

    /// Gives up the connection. Its thread closes the socket when it ends.
    void abandon() noexcept
    {
        if (state_ == nullptr) {
            return;
        }
        {
            std::lock_guard<std::mutex> lock{state_->mutex};
            state_->isAbandoned = true;
            if (state_->result.has_value() && state_->result->has_value()) {
                state_->closeSocket(**state_->result);
            }
            state_->result.reset();
        }
        state_ = nullptr;
    }

private:
    using Clock = std::chrono::steady_clock;

    /// State is what the owner and the thread share.
    struct State final {
        std::mutex mutex;
        std::optional<Result> result;
        CloseFunction closeSocket = nullptr;
        bool isAbandoned = false;
    };

    std::shared_ptr<State> state_;
    Clock::time_point startTime_;
    Duration timeout_ = Duration::zero();
    std::string errorContext_;
};

} // namespace pomdog::network::detail
