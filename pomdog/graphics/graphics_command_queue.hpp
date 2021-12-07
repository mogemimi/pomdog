// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/graphics/forward_declarations.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdlib>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

class GraphicsCommandList;

class POMDOG_EXPORT GraphicsCommandQueue {
public:
    GraphicsCommandQueue() noexcept;
    GraphicsCommandQueue(const GraphicsCommandQueue&) = delete;
    GraphicsCommandQueue& operator=(const GraphicsCommandQueue&) = delete;

    virtual ~GraphicsCommandQueue();

    virtual std::size_t GetCommandCount() const noexcept = 0;

    virtual void Reset() = 0;

    virtual void PushbackCommandList(const std::shared_ptr<GraphicsCommandList>& commandList) = 0;

    virtual void ExecuteCommandLists() = 0;

    virtual void Present() = 0;
};

} // namespace Pomdog
