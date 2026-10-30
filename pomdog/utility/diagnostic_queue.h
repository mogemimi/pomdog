// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <mutex>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail {

/// DiagnosticQueue collects diagnostics from engine subsystems and delivers
/// them at frame boundaries.
///
/// A diagnostic reports a failure that a subsystem already handled while
/// keeping a safe state, such as a gamepad that could not be opened and is
/// skipped. A subsystem receives a shared queue and calls enqueue() instead of
/// storing or writing diagnostics itself. The host calls emit() once per frame
/// and passes each diagnostic to the application. A subsystem that can report
/// the same diagnostic repeatedly limits its own reports.
///
/// enqueue() may be called from any thread, including from inside the emit()
/// callback. emit() is not reentrant and must be called from a single
/// consumer thread, in practice the game loop thread.
class DiagnosticQueue final {
private:
    std::vector<std::unique_ptr<Error>> diagnostics_;
    std::vector<std::unique_ptr<Error>> notifications_;
    std::mutex mutex_;

public:
    DiagnosticQueue();

    DiagnosticQueue(const DiagnosticQueue&) = delete;
    DiagnosticQueue(DiagnosticQueue&&) = delete;
    DiagnosticQueue& operator=(const DiagnosticQueue&) = delete;
    DiagnosticQueue& operator=(DiagnosticQueue&&) = delete;

    ~DiagnosticQueue();

    /// Appends the diagnostic, which must not be null.
    void enqueue(std::unique_ptr<Error> diagnostic);

    /// Drains the diagnostics enqueued so far and invokes the callback for
    /// each one in enqueue order. Diagnostics enqueued while emit() is
    /// running, including those enqueued by the callback itself, are
    /// delivered on the next emit() call.
    void emit(const std::function<void(std::unique_ptr<Error> diagnostic)>& callback);
};

} // namespace pomdog::detail
