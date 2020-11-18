// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include <cstdlib>
#include <memory>

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
