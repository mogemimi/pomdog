// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/backends/system_events.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <functional>
#include <mutex>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

/// Thread-safe event queue for dispatching system events to game host internals.
///
/// Events are enqueued from platform-specific code (e.g. window procedures,
/// input device callbacks) and emitted on the game loop thread. The queue is
/// double-buffered: enqueue() appends to the back buffer while emit() swaps
/// and drains the front buffer, so producers and consumers never contend on the
/// same container.
///
/// enqueue() may be called from any thread, including from inside the emit()
/// callback. emit() is not reentrant and must be called from a single
/// consumer thread, in practice the game loop thread.
class SystemEventQueue final {
private:
    std::vector<SystemEvent> events_;
    std::vector<SystemEvent> notifications_;
    std::mutex mutex_;

public:
    SystemEventQueue();

    SystemEventQueue(const SystemEventQueue&) = delete;
    SystemEventQueue(SystemEventQueue&&) = delete;
    SystemEventQueue& operator=(const SystemEventQueue&) = delete;
    SystemEventQueue& operator=(SystemEventQueue&&) = delete;

    /// Enqueues a copy of the event.
    void enqueue(const SystemEvent& event);

    /// Enqueues the event by moving it.
    void enqueue(SystemEvent&& event);

    /// Drains the events queued so far and invokes the callback for each one
    /// in enqueue order. Events enqueued while emit() is running, including
    /// those enqueued by the callback itself, are delivered on the next
    /// emit() call.
    void emit(const std::function<void(SystemEvent event)>& callback);

    /// Reserves capacity for at least `capacity` events in both internal
    /// buffers. Intended for setup; not safe to call concurrently with
    /// emit().
    void reserve(std::size_t capacity);
};

} // namespace pomdog::detail
