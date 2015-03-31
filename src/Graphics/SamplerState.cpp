// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Graphics/SamplerState.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeSamplerState.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
SamplerState::SamplerState(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    SamplerDescription const& description)
    : nativeSamplerState(graphicsDevice->NativeGraphicsDevice()
        ->CreateSamplerState(description))
{
    POMDOG_ASSERT(nativeSamplerState);
}
//-----------------------------------------------------------------------
SamplerState::~SamplerState() = default;
//-----------------------------------------------------------------------
Detail::RenderSystem::NativeSamplerState* SamplerState::NativeSamplerState()
{
    return nativeSamplerState.get();
}
//-----------------------------------------------------------------------
} // namespace Pomdog
