// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Pomdog/Graphics/EffectReflection.hpp"
#include "../RenderSystem/NativeEffectReflection.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/PipelineState.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
EffectReflection::EffectReflection(
    GraphicsDevice & graphicsDevice,
    PipelineStateDescription const& pipelineStateDescription,
    std::shared_ptr<PipelineState> const& pipelineState)
{
    auto nativeDevice = graphicsDevice.NativeGraphicsDevice();

    POMDOG_ASSERT(nativeDevice != nullptr);
    POMDOG_ASSERT(pipelineState);
    POMDOG_ASSERT(pipelineState->NativePipelineState() != nullptr);

    nativeEffectReflection = nativeDevice->CreateEffectReflection(
        pipelineStateDescription,
        *pipelineState->NativePipelineState());
}
//-----------------------------------------------------------------------
EffectReflection::EffectReflection(
    std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    PipelineStateDescription const& pipelineStateDescription,
    std::shared_ptr<PipelineState> const& pipelineState)
    : EffectReflection(
        *graphicsDevice,
        pipelineStateDescription,
        pipelineState)
{}
//-----------------------------------------------------------------------
EffectReflection::~EffectReflection() = default;
//-----------------------------------------------------------------------
std::vector<EffectConstantDescription> EffectReflection::GetConstantBuffers() const
{
    POMDOG_ASSERT(nativeEffectReflection);
    return nativeEffectReflection->GetConstantBuffers();
}
//-----------------------------------------------------------------------
} // namespace Pomdog
