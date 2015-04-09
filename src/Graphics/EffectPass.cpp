// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Graphics/EffectPass.hpp"
#include "../RenderSystem/NativePipelineState.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/PipelineStateDescription.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
EffectPass::EffectPass(GraphicsDevice & graphicsDevice,
    PipelineStateDescription const& description)
{
    auto nativeDevice = graphicsDevice.NativeGraphicsDevice();

    POMDOG_ASSERT(nativeDevice);
    nativePipelineState = nativeDevice->CreatePipelineState(description);
}
//-----------------------------------------------------------------------
EffectPass::EffectPass(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    PipelineStateDescription const& description)
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
