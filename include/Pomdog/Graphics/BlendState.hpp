// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_BLENDSTATE_CFC118A6_HPP
#define POMDOG_BLENDSTATE_CFC118A6_HPP

#include "detail/ForwardDeclarations.hpp"
#include "BlendDescription.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeBlendState;

}// namespace RenderSystem
}// namespace Detail

class POMDOG_EXPORT BlendState {
public:
    BlendState() = delete;
    BlendState(BlendState const&) = delete;
    BlendState & operator=(BlendState const&) = delete;

    BlendState(std::shared_ptr<GraphicsDevice> const& graphicsDevice, BlendDescription const& description);

    ~BlendState();

    BlendDescription Description() const;

    static std::shared_ptr<BlendState>
    CreateAdditive(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

    static std::shared_ptr<BlendState>
    CreateAlphaBlend(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

    static std::shared_ptr<BlendState>
    CreateNonPremultiplied(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

    static std::shared_ptr<BlendState>
    CreateOpaque(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

public:
    Detail::RenderSystem::NativeBlendState* NativeBlendState();

private:
    std::unique_ptr<Detail::RenderSystem::NativeBlendState> nativeBlendState;
    BlendDescription description;
};

}// namespace Pomdog

#endif // POMDOG_BLENDSTATE_CFC118A6_HPP
