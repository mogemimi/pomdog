// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_NATIVEGRAPHICSCOMMANDQUEUE_A41D671F_HPP
#define POMDOG_NATIVEGRAPHICSCOMMANDQUEUE_A41D671F_HPP

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>
#include <cstdlib>

namespace Pomdog {
namespace Detail {

class NativeGraphicsCommandQueue {
public:
    NativeGraphicsCommandQueue() = default;
    NativeGraphicsCommandQueue(NativeGraphicsCommandQueue const&) = delete;
    NativeGraphicsCommandQueue & operator=(NativeGraphicsCommandQueue const&) = delete;

    virtual ~NativeGraphicsCommandQueue() = default;

    virtual void Reset() = 0;

    virtual void PushbackCommandList(std::shared_ptr<GraphicsCommandList> const& commandList) = 0;

    virtual void ExecuteCommandLists() = 0;

    virtual void Present() = 0;

    virtual std::size_t GetCommandCount() const noexcept = 0;
};

} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_NATIVEGRAPHICSCOMMANDQUEUE_A41D671F_HPP
