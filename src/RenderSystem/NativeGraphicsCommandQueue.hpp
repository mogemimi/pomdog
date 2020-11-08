// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include <cstdlib>
#include <memory>

namespace Pomdog::Detail {

class NativeGraphicsCommandQueue {
public:
    NativeGraphicsCommandQueue() = default;
    NativeGraphicsCommandQueue(const NativeGraphicsCommandQueue&) = delete;
    NativeGraphicsCommandQueue& operator=(const NativeGraphicsCommandQueue&) = delete;

    virtual ~NativeGraphicsCommandQueue() = default;

    virtual void Reset() = 0;

    virtual void PushbackCommandList(const std::shared_ptr<GraphicsCommandList>& commandList) = 0;

    virtual void ExecuteCommandLists() = 0;

    virtual void Present() = 0;

    virtual std::size_t GetCommandCount() const noexcept = 0;
};

} // namespace Pomdog::Detail
