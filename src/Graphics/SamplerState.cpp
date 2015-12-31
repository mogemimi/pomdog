// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Pomdog/Graphics/SamplerState.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeSamplerState.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
SamplerState::SamplerState(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    SamplerDescription const& description)
    : nativeSamplerState(graphicsDevice->GetNativeGraphicsDevice()
        ->CreateSamplerState(description))
{
    POMDOG_ASSERT(nativeSamplerState);
}
//-----------------------------------------------------------------------
SamplerState::~SamplerState() = default;
//-----------------------------------------------------------------------
Detail::NativeSamplerState* SamplerState::GetNativeSamplerState()
{
    return nativeSamplerState.get();
}
//-----------------------------------------------------------------------
} // namespace Pomdog
