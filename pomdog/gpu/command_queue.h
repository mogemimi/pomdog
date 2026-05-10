// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class CommandList;
} // namespace pomdog::gpu

namespace pomdog::gpu {

/// CommandQueue submits recorded command lists to the GPU for execution.
class POMDOG_EXPORT CommandQueue {
public:
    CommandQueue() noexcept;
    CommandQueue(const CommandQueue&) = delete;
    CommandQueue& operator=(const CommandQueue&) = delete;

    virtual ~CommandQueue();

    /// Clears all pending command lists from the queue.
    virtual void reset() = 0;

    /// Appends a recorded command list to the end of the submission queue.
    virtual void pushBackCommandList(const std::shared_ptr<CommandList>& commandList) = 0;

    /// Submits all queued command lists to the GPU for execution.
    virtual void executeCommandLists() = 0;

    /// Presents the rendered back buffer to the display.
    virtual void present() = 0;
};

} // namespace pomdog::gpu
