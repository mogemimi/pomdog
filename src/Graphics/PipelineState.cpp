// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Graphics/PipelineState.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativePipelineState.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/PipelineStateDescription.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {

PipelineState::PipelineState(
    GraphicsDevice & graphicsDevice,
    const PipelineStateDescription& description)
{
    auto nativeDevice = graphicsDevice.GetNativeGraphicsDevice();

    POMDOG_ASSERT(nativeDevice);
    nativePipelineState = nativeDevice->CreatePipelineState(description);
}

PipelineState::PipelineState(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    const PipelineStateDescription& description)
    : PipelineState(*graphicsDevice, description)
{
}

PipelineState::~PipelineState() = default;

Detail::NativePipelineState* PipelineState::GetNativePipelineState()
{
    return nativePipelineState.get();
}

} // namespace Pomdog
