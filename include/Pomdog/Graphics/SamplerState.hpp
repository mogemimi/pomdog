// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SAMPLERSTATE_87568CD5_HPP
#define POMDOG_SAMPLERSTATE_87568CD5_HPP

#include "detail/ForwardDeclarations.hpp"
#include "SamplerDescription.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeSamplerState;

}// namespace RenderSystem
}// namespace Detail

class GraphicsDevice;

class POMDOG_EXPORT SamplerState {
public:
    SamplerState() = delete;
    SamplerState(SamplerState const&) = delete;
    SamplerState & operator=(SamplerState const&) = delete;

    SamplerState(std::shared_ptr<GraphicsDevice> const& graphicsDevice, SamplerDescription const& description);

    ~SamplerState();

    SamplerDescription Description() const;

    static std::shared_ptr<SamplerState>
    CreateAnisotropicClamp(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

    static std::shared_ptr<SamplerState>
    CreateAnisotropicWrap(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

    static std::shared_ptr<SamplerState>
    CreateLinearClamp(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

    static std::shared_ptr<SamplerState>
    CreateLinearWrap(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

    static std::shared_ptr<SamplerState>
    CreatePointClamp(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

    static std::shared_ptr<SamplerState>
    CreatePointWrap(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

public:
    Detail::RenderSystem::NativeSamplerState* NativeSamplerState();

private:
    std::unique_ptr<Detail::RenderSystem::NativeSamplerState> nativeSamplerState;
    SamplerDescription description;
};

}// namespace Pomdog

#endif // POMDOG_SAMPLERSTATE_87568CD5_HPP
