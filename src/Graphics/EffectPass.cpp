// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Graphics/EffectPass.hpp"
#include "../RenderSystem/NativePipelineState.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "Pomdog/Graphics/EffectPassDescription.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
EffectPass::EffectPass(GraphicsDevice & graphicsDevice,
    EffectPassDescription const& description)
{
    auto nativeDevice = graphicsDevice.NativeGraphicsDevice();

    POMDOG_ASSERT(nativeDevice);
    nativePipelineState = nativeDevice->CreatePipelineState(description);
}
//-----------------------------------------------------------------------
EffectPass::EffectPass(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    EffectPassDescription const& description)
    : EffectPass(*graphicsDevice, description)
{}
//-----------------------------------------------------------------------
EffectPass::~EffectPass() = default;
//-----------------------------------------------------------------------
Detail::RenderSystem::NativePipelineState* EffectPass::NativePipelineState()
{
    return nativePipelineState.get();
}
//-----------------------------------------------------------------------
} // namespace Pomdog
