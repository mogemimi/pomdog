// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdlib>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

class POMDOG_EXPORT CommandQueue {
public:
    CommandQueue() noexcept;
    CommandQueue(const CommandQueue&) = delete;
    CommandQueue& operator=(const CommandQueue&) = delete;

    virtual ~CommandQueue();

    [[nodiscard]] virtual std::size_t GetCommandCount() const noexcept = 0;

    virtual void Reset() = 0;

    virtual void PushbackCommandList(const std::shared_ptr<CommandList>& commandList) = 0;

    virtual void ExecuteCommandLists() = 0;

    virtual void Present() = 0;
};

} // namespace pomdog::gpu
