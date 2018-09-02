// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstdlib>
#include <memory>

namespace Pomdog {

class GraphicsCommandList;

class POMDOG_EXPORT GraphicsCommandQueue final {
public:
    GraphicsCommandQueue() = delete;
    GraphicsCommandQueue(const GraphicsCommandQueue&) = delete;
    GraphicsCommandQueue& operator=(const GraphicsCommandQueue&) = delete;

    explicit GraphicsCommandQueue(
        std::unique_ptr<Detail::NativeGraphicsCommandQueue>&& nativeCommandQueue);

    std::size_t GetCommandCount() const noexcept;

    void Reset();

    void PushbackCommandList(const std::shared_ptr<GraphicsCommandList>& commandList);

    void ExecuteCommandLists();

    void Present();

private:
    std::unique_ptr<Detail::NativeGraphicsCommandQueue> nativeCommandQueue;
};

} // namespace Pomdog
