// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <cstdlib>

namespace Pomdog {

class GraphicsCommandList;

class POMDOG_EXPORT GraphicsCommandQueue {
public:
    GraphicsCommandQueue() = delete;
    GraphicsCommandQueue(GraphicsCommandQueue const&) = delete;
    GraphicsCommandQueue & operator=(GraphicsCommandQueue const&) = delete;

    explicit GraphicsCommandQueue(
        std::unique_ptr<Detail::NativeGraphicsCommandQueue> && nativeCommandQueue);

    std::size_t GetCommandCount() const noexcept;

    void Reset();

    void PushbackCommandList(std::shared_ptr<GraphicsCommandList> const& commandList);

    void ExecuteCommandLists();

    void Present();

private:
    std::unique_ptr<Detail::NativeGraphicsCommandQueue> nativeCommandQueue;
};

} // namespace Pomdog
