// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_DEPTHSTENCILSTATE_8D3E62B6_HPP
#define POMDOG_DEPTHSTENCILSTATE_8D3E62B6_HPP

#include "detail/ForwardDeclarations.hpp"
#include "DepthStencilDescription.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeDepthStencilState;

}// namespace RenderSystem
}// namespace Detail

class POMDOG_EXPORT DepthStencilState {
public:
    DepthStencilState() = delete;
    DepthStencilState(DepthStencilState const&) = delete;
    DepthStencilState & operator=(DepthStencilState const&) = delete;

    DepthStencilState(std::shared_ptr<GraphicsDevice> const& graphicsDevice, DepthStencilDescription const& description);

    ~DepthStencilState();

    DepthStencilDescription Description() const;

    static std::shared_ptr<DepthStencilState>
    CreateReadWriteDepth(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

    static std::shared_ptr<DepthStencilState>
    CreateReadOnlyDepth(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

    static std::shared_ptr<DepthStencilState>
    CreateNone(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

public:
    Detail::RenderSystem::NativeDepthStencilState* NativeDepthStencilState();

private:
    std::unique_ptr<Detail::RenderSystem::NativeDepthStencilState> nativeDepthStencilState;
    DepthStencilDescription description;
};

}// namespace Pomdog

#endif // POMDOG_DEPTHSTENCILSTATE_8D3E62B6_HPP
