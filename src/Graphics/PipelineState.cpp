// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#include "Pomdog/Graphics/PipelineState.hpp"
#include "../RenderSystem/NativePipelineState.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/PipelineStateDescription.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
PipelineState::PipelineState(GraphicsDevice & graphicsDevice,
    PipelineStateDescription const& description)
{
    auto nativeDevice = graphicsDevice.NativeGraphicsDevice();

    POMDOG_ASSERT(nativeDevice);
    nativePipelineState = nativeDevice->CreatePipelineState(description);
}
//-----------------------------------------------------------------------
PipelineState::PipelineState(
    std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    PipelineStateDescription const& description)
    : PipelineState(*graphicsDevice, description)
{}
//-----------------------------------------------------------------------
PipelineState::~PipelineState() = default;
//-----------------------------------------------------------------------
Detail::NativePipelineState* PipelineState::NativePipelineState()
{
    return nativePipelineState.get();
}
//-----------------------------------------------------------------------
} // namespace Pomdog
