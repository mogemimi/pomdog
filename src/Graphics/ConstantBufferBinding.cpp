// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Graphics/ConstantBufferBinding.hpp"
#include "../RenderSystem/NativeBuffer.hpp"
#include "../RenderSystem/NativeConstantLayout.hpp"
#include "../RenderSystem/NativeEffectPass.hpp"
#include "../RenderSystem/NativeEffectReflection.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "Pomdog/Graphics/EffectPass.hpp"
#include "Pomdog/Graphics/EffectConstantDescription.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace {

static auto dummyParameter = std::make_shared<ConstantBuffer>();

} // unnamed namespace
//-----------------------------------------------------------------------
ConstantBufferBinding::ConstantBufferBinding(GraphicsDevice & graphicsDevice,
    EffectPass & effectPass)
{
    auto nativeDevice = graphicsDevice.NativeGraphicsDevice();

    auto nativeEffectPass = effectPass.NativeEffectPass();

    // Create effect reflection:
    POMDOG_ASSERT(nativeEffectPass);
    auto effectReflection = nativeDevice->CreateEffectReflection(*nativeEffectPass);

    POMDOG_ASSERT(effectReflection);
    auto constants = effectReflection->GetConstantBuffers();

    // Create constant buffers:
    for (auto & desc: constants)
    {
        auto constantBuffer = std::make_shared<ConstantBuffer>(
            graphicsDevice, desc.ByteSize);
        constantBuffers[desc.Name] = std::move(constantBuffer);
    }

    nativeConstantLayout = nativeEffectPass->CreateConstantLayout();

    // Bind constant buffers:
    for (auto & parameter: constantBuffers)
    {
        using Detail::RenderSystem::NativeBuffer;
        std::shared_ptr<NativeBuffer> nativeConstantBuffer(
            parameter.second, parameter.second->NativeConstantBuffer());

        POMDOG_ASSERT(nativeConstantLayout);
        nativeConstantLayout->SetConstantBuffer(
            parameter.first, nativeConstantBuffer);
    }
}
//-----------------------------------------------------------------------
ConstantBufferBinding::ConstantBufferBinding(
    std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    EffectPass & effectPass)
    : ConstantBufferBinding(*graphicsDevice, effectPass)
{
}
//-----------------------------------------------------------------------
ConstantBufferBinding::~ConstantBufferBinding() = default;
//-----------------------------------------------------------------------
std::shared_ptr<ConstantBuffer> const& ConstantBufferBinding::Find(
    std::string const& parameterName) const
{
    POMDOG_ASSERT(!parameterName.empty());
    POMDOG_ASSERT(!constantBuffers.empty());

    auto iter = constantBuffers.find(parameterName);
    if (iter != std::end(constantBuffers)) {
        return iter->second;
    }
    return dummyParameter;
}
//-----------------------------------------------------------------------
ConstantBufferCollection const& ConstantBufferBinding::Find() const
{
    return constantBuffers;
}
//-----------------------------------------------------------------------
Detail::RenderSystem::NativeConstantLayout* ConstantBufferBinding::NativeConstantLayout()
{
    return nativeConstantLayout.get();
}
//-----------------------------------------------------------------------
} // namespace Pomdog
