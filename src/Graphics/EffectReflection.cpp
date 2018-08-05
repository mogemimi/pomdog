// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Graphics/EffectReflection.hpp"
#include "../RenderSystem/NativeEffectReflection.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/PipelineState.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {

EffectReflection::EffectReflection(
    GraphicsDevice & graphicsDevice,
    const PipelineStateDescription& pipelineStateDescription,
    const std::shared_ptr<PipelineState>& pipelineState)
{
    auto nativeDevice = graphicsDevice.GetNativeGraphicsDevice();

    POMDOG_ASSERT(nativeDevice != nullptr);
    POMDOG_ASSERT(pipelineState);
    POMDOG_ASSERT(pipelineState->GetNativePipelineState() != nullptr);

    nativeEffectReflection = nativeDevice->CreateEffectReflection(
        pipelineStateDescription,
        *pipelineState->GetNativePipelineState());
}

EffectReflection::EffectReflection(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    const PipelineStateDescription& pipelineStateDescription,
    const std::shared_ptr<PipelineState>& pipelineState)
    : EffectReflection(
        *graphicsDevice,
        pipelineStateDescription,
        pipelineState)
{
}

EffectReflection::~EffectReflection() = default;

std::vector<EffectConstantDescription> EffectReflection::GetConstantBuffers() const
{
    POMDOG_ASSERT(nativeEffectReflection);
    return nativeEffectReflection->GetConstantBuffers();
}

} // namespace Pomdog
