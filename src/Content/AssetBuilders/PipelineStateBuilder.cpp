// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Pomdog/Content/AssetBuilders/PipelineStateBuilder.hpp"
#include "Pomdog/Content/detail/AssetLoaderContext.hpp"
#include "Pomdog/Graphics/EffectReflection.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/PipelineState.hpp"
#include "Pomdog/Graphics/PipelineStateDescription.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <utility>

namespace Pomdog {
namespace AssetBuilders {
//-----------------------------------------------------------------------
class Builder<PipelineState>::Impl final {
public:
    PipelineStateDescription description;
    Detail::AssetLoaderContext loaderContext;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    bool hasBlendState;
    bool hasRasterizerState;
    bool hasDepthStencilState;

    Impl();

public:
    std::shared_ptr<PipelineState> Load();
};
//-----------------------------------------------------------------------
Builder<PipelineState>::Impl::Impl()
{
    description.MultiSampleMask = std::numeric_limits<std::uint32_t>::max();
    hasBlendState = false;
    hasRasterizerState = false;
    hasDepthStencilState = false;
}
//-----------------------------------------------------------------------
std::shared_ptr<PipelineState> Builder<PipelineState>::Impl::Load()
{
    POMDOG_ASSERT(!description.InputLayout.InputElements.empty());

    if (!hasBlendState) {
        description.BlendState = BlendDescription::CreateDefault();
        hasBlendState = true;
    }

    if (!hasRasterizerState) {
        description.RasterizerState = RasterizerDescription::CreateDefault();
        hasRasterizerState = true;
    }

    if (!hasDepthStencilState) {
        description.DepthStencilState = DepthStencilDescription::CreateDefault();
        hasDepthStencilState = true;
    }

    auto pipelineState = std::make_shared<PipelineState>(graphicsDevice, description);
    return std::move(pipelineState);
}
//-----------------------------------------------------------------------
// explicit instantiations
template class Builder<PipelineState>;
//-----------------------------------------------------------------------
Builder<PipelineState>::Builder(Detail::AssetLoaderContext const& contextIn)
    : impl(std::make_unique<Impl>())
{
    POMDOG_ASSERT(impl);
    impl->loaderContext = contextIn;
    impl->graphicsDevice = contextIn.GraphicsDevice.lock();
    POMDOG_ASSERT(impl->graphicsDevice);
}
//-----------------------------------------------------------------------
Builder<PipelineState>::Builder(Builder<PipelineState> &&) = default;
Builder<PipelineState> & Builder<PipelineState>::operator=(Builder<PipelineState> &&) = default;
Builder<PipelineState>::~Builder() = default;
//-----------------------------------------------------------------------
Builder<PipelineState> & Builder<PipelineState>::SetVertexShader(
    std::shared_ptr<Shader> const& vertexShader)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(vertexShader);
    impl->description.VertexShader = vertexShader;
    return *this;
}
//-----------------------------------------------------------------------
Builder<PipelineState> & Builder<PipelineState>::SetVertexShader(
    std::shared_ptr<Shader> && vertexShader)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(vertexShader);
    impl->description.VertexShader = std::move(vertexShader);
    return *this;
}
//-----------------------------------------------------------------------
Builder<PipelineState> & Builder<PipelineState>::SetPixelShader(
    std::shared_ptr<Shader> const& pixelShader)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(pixelShader);
    impl->description.PixelShader = pixelShader;
    return *this;
}
//-----------------------------------------------------------------------
Builder<PipelineState> & Builder<PipelineState>::SetPixelShader(
    std::shared_ptr<Shader> && pixelShader)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(pixelShader);
    impl->description.PixelShader = std::move(pixelShader);
    return *this;
}
//-----------------------------------------------------------------------
Builder<PipelineState> & Builder<PipelineState>::SetInputLayout(
    InputLayoutDescription const& inputLayout)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(!inputLayout.InputElements.empty());
    impl->description.InputLayout = inputLayout;
    return *this;
}
//-----------------------------------------------------------------------
Builder<PipelineState> & Builder<PipelineState>::SetInputLayout(
    InputLayoutDescription && inputLayout)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(!inputLayout.InputElements.empty());
    impl->description.InputLayout = std::move(inputLayout);
    return *this;
}
//-----------------------------------------------------------------------
Builder<PipelineState> & Builder<PipelineState>::SetBlendState(
    BlendDescription const& blendState)
{
    POMDOG_ASSERT(impl);
    impl->description.BlendState = blendState;
    impl->hasBlendState = true;
    return *this;
}
//-----------------------------------------------------------------------
Builder<PipelineState> & Builder<PipelineState>::SetRasterizerState(
    RasterizerDescription const& rasterizerState)
{
    POMDOG_ASSERT(impl);
    impl->description.RasterizerState = rasterizerState;
    impl->hasRasterizerState = true;
    return *this;
}
//-----------------------------------------------------------------------
Builder<PipelineState> & Builder<PipelineState>::SetDepthStencilState(
    DepthStencilDescription const& depthStencilState)
{
    POMDOG_ASSERT(impl);
    impl->description.DepthStencilState = depthStencilState;
    impl->hasDepthStencilState = true;
    return *this;
}
//-----------------------------------------------------------------------
Builder<PipelineState> & Builder<PipelineState>::SetConstantBufferBindSlot(
    std::string const& name, int slotIndex)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(!name.empty());
    POMDOG_ASSERT(slotIndex >= 0);

#if defined(DEBUG) && !defined(NDEBUG)
    for (auto & pair : impl->description.ConstantBufferBindSlots) {
        POMDOG_ASSERT(slotIndex != pair.second);
    }
#endif

    impl->description.ConstantBufferBindSlots.emplace(name, slotIndex);
    return *this;
}
//-----------------------------------------------------------------------
std::shared_ptr<PipelineState> Builder<PipelineState>::Build()
{
    POMDOG_ASSERT(impl);
    return impl->Load();
}
//-----------------------------------------------------------------------
std::shared_ptr<EffectReflection> Builder<PipelineState>::CreateEffectReflection(
    std::shared_ptr<PipelineState> const& pipelineState)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(pipelineState);

    auto effectReflection = std::make_shared<EffectReflection>(
        impl->graphicsDevice,
        impl->description,
        pipelineState);
    return std::move(effectReflection);
}
//-----------------------------------------------------------------------
PipelineStateDescription const& Builder<PipelineState>::GetDescription() const
{
    POMDOG_ASSERT(impl);
    return impl->description;
}
//-----------------------------------------------------------------------
} // namespace AssetBuilders
} // namespace Pomdog
