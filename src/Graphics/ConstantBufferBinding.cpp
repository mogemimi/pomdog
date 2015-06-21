// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Graphics/ConstantBufferBinding.hpp"
#include "../RenderSystem/NativeBuffer.hpp"
#include "../RenderSystem/NativeConstantLayout.hpp"
#include "../RenderSystem/NativePipelineState.hpp"
#include "../RenderSystem/NativeEffectReflection.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "Pomdog/Graphics/EffectConstantDescription.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/PipelineState.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
ConstantBufferBinding::ConstantBufferBinding(GraphicsDevice & graphicsDevice,
    PipelineStateDescription const& pipelineStateDescription,
    PipelineState & pipelineState)
{
    auto nativeDevice = graphicsDevice.NativeGraphicsDevice();

    auto nativePipelineState = pipelineState.NativePipelineState();

    // Get effect reflection
    POMDOG_ASSERT(nativePipelineState);
    auto effectReflection = nativeDevice->CreateEffectReflection(
        pipelineStateDescription,
        *nativePipelineState);

    POMDOG_ASSERT(effectReflection);
    auto constants = effectReflection->GetConstantBuffers();

    // Create constant buffers
    for (auto & desc: constants)
    {
        auto constantBuffer = std::make_shared<ConstantBuffer>(
            graphicsDevice, desc.ByteSize, BufferUsage::Dynamic);

        constantBuffers[desc.Name] = std::move(constantBuffer);
    }

    nativeConstantLayout = nativePipelineState->CreateConstantLayout();

    // Bind constant buffers
    for (auto & pair: constantBuffers)
    {
        auto & name = pair.first;
        std::shared_ptr<Detail::NativeBuffer> constantBuffer(
            pair.second, pair.second->NativeConstantBuffer());

        POMDOG_ASSERT(nativeConstantLayout);
        nativeConstantLayout->SetConstantBuffer(name, constantBuffer);
    }
}
//-----------------------------------------------------------------------
ConstantBufferBinding::ConstantBufferBinding(
    std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    PipelineStateDescription const& pipelineStateDescription,
    PipelineState & pipelineState)
    : ConstantBufferBinding(
        *graphicsDevice,
        pipelineStateDescription,
        pipelineState)
{
}
//-----------------------------------------------------------------------
ConstantBufferBinding::~ConstantBufferBinding() = default;
//-----------------------------------------------------------------------
std::shared_ptr<ConstantBuffer> ConstantBufferBinding::FindConstantBuffer(
    std::string const& name) const
{
    POMDOG_ASSERT(!name.empty());
    POMDOG_ASSERT(!constantBuffers.empty());

    auto iter = constantBuffers.find(name);
    if (iter != std::end(constantBuffers)) {
        return iter->second;
    }
    return {};
}
//-----------------------------------------------------------------------
void ConstantBufferBinding::SetConstantBuffer(
    std::string const& name,
    std::shared_ptr<ConstantBuffer> const& constantBuffer)
{
    POMDOG_ASSERT(!name.empty());
    POMDOG_ASSERT(!constantBuffers.empty());

    auto iter = constantBuffers.find(name);
    POMDOG_ASSERT(iter != std::end(constantBuffers));
    if (iter == std::end(constantBuffers)) {
        return;
    }

    iter->second = constantBuffer;

    POMDOG_ASSERT(constantBuffer->NativeConstantBuffer() != nullptr);
    std::shared_ptr<Detail::NativeBuffer> nativeBuffer(
        constantBuffer, constantBuffer->NativeConstantBuffer());

    POMDOG_ASSERT(nativeBuffer);
    nativeConstantLayout->SetConstantBuffer(name, nativeBuffer);
}
//-----------------------------------------------------------------------
ConstantBufferCollection const& ConstantBufferBinding::GetConstantBuffers() const
{
    return constantBuffers;
}
//-----------------------------------------------------------------------
Detail::NativeConstantLayout* ConstantBufferBinding::NativeConstantLayout()
{
    return nativeConstantLayout.get();
}
//-----------------------------------------------------------------------
} // namespace Pomdog
