// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Content/AssetBuilders/PipelineStateBuilder.hpp"
#include "Pomdog/Content/detail/AssetLoaderContext.hpp"
#include "Pomdog/Graphics/EffectReflection.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/PipelineState.hpp"
#include "Pomdog/Graphics/PipelineStateDescription.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <utility>

namespace Pomdog {
namespace AssetBuilders {

class Builder<PipelineState>::Impl final {
public:
    PipelineStateDescription description;
    Detail::AssetLoaderContext loaderContext;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    bool hasBlendState;
    bool hasRasterizerState;
    bool hasDepthStencilState;
    bool hasRenderTargetViewFormats;
    bool hasDepthStencilViewFormat;

    Impl();

public:
    std::shared_ptr<PipelineState> Load();
};

Builder<PipelineState>::Impl::Impl()
{
    description.MultiSampleMask = std::numeric_limits<std::uint32_t>::max();
    hasBlendState = false;
    hasRasterizerState = false;
    hasDepthStencilState = false;
    hasRenderTargetViewFormats = false;
    hasDepthStencilViewFormat = false;
}

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

    POMDOG_ASSERT(hasRenderTargetViewFormats);
    POMDOG_ASSERT(hasDepthStencilViewFormat);

    if (!hasRenderTargetViewFormats) {
#if defined(DEBUG) && !defined(NDEBUG)
        Log::Warning(
            "Pomdog",
            "[Warning] Please call Builder<PipelineState>::SetRenderTargetViewFormats() in your code.");
#endif
        description.RenderTargetViewFormats = { SurfaceFormat::R8G8B8A8_UNorm };
        hasRenderTargetViewFormats = true;
    }

    if (!hasDepthStencilViewFormat) {
#if defined(DEBUG) && !defined(NDEBUG)
        Log::Warning(
            "Pomdog",
            "[Warning] Please call Builder<PipelineState>::SetDepthStencilViewFormat() in your code.");
#endif
        description.DepthStencilViewFormat = DepthFormat::None;
        hasDepthStencilViewFormat = true;
    }

    auto pipelineState = std::make_shared<PipelineState>(graphicsDevice, description);
    return pipelineState;
}

Builder<PipelineState>::Builder(const Detail::AssetLoaderContext& contextIn)
    : impl(std::make_unique<Impl>())
{
    POMDOG_ASSERT(impl);
    impl->loaderContext = contextIn;
    impl->graphicsDevice = contextIn.GraphicsDevice.lock();
    POMDOG_ASSERT(impl->graphicsDevice);
}

Builder<PipelineState>::Builder(Builder<PipelineState> &&) = default;
Builder<PipelineState> & Builder<PipelineState>::operator=(Builder<PipelineState> &&) = default;
Builder<PipelineState>::~Builder() = default;

Builder<PipelineState> & Builder<PipelineState>::SetVertexShader(
    const std::shared_ptr<Shader>& vertexShader)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(vertexShader);
    impl->description.VertexShader = vertexShader;
    return *this;
}

Builder<PipelineState> & Builder<PipelineState>::SetVertexShader(
    std::shared_ptr<Shader> && vertexShader)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(vertexShader);
    impl->description.VertexShader = std::move(vertexShader);
    return *this;
}

Builder<PipelineState> & Builder<PipelineState>::SetPixelShader(
    const std::shared_ptr<Shader>& pixelShader)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(pixelShader);
    impl->description.PixelShader = pixelShader;
    return *this;
}

Builder<PipelineState> & Builder<PipelineState>::SetPixelShader(
    std::shared_ptr<Shader> && pixelShader)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(pixelShader);
    impl->description.PixelShader = std::move(pixelShader);
    return *this;
}

Builder<PipelineState> & Builder<PipelineState>::SetInputLayout(
    const InputLayoutDescription& inputLayout)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(!inputLayout.InputElements.empty());
    impl->description.InputLayout = inputLayout;
    return *this;
}

Builder<PipelineState> & Builder<PipelineState>::SetInputLayout(
    InputLayoutDescription && inputLayout)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(!inputLayout.InputElements.empty());
    impl->description.InputLayout = std::move(inputLayout);
    return *this;
}

Builder<PipelineState> & Builder<PipelineState>::SetBlendState(
    const BlendDescription& blendState)
{
    POMDOG_ASSERT(impl);
    impl->description.BlendState = blendState;
    impl->hasBlendState = true;
    return *this;
}

Builder<PipelineState> & Builder<PipelineState>::SetRasterizerState(
    const RasterizerDescription& rasterizerState)
{
    POMDOG_ASSERT(impl);
    impl->description.RasterizerState = rasterizerState;
    impl->hasRasterizerState = true;
    return *this;
}

Builder<PipelineState> & Builder<PipelineState>::SetDepthStencilState(
    const DepthStencilDescription& depthStencilState)
{
    POMDOG_ASSERT(impl);
    impl->description.DepthStencilState = depthStencilState;
    impl->hasDepthStencilState = true;
    return *this;
}

Builder<PipelineState> & Builder<PipelineState>::SetConstantBufferBindSlot(
    const std::string& name, int slotIndex)
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

Builder<PipelineState> & Builder<PipelineState>::SetRenderTargetViewFormat(
    SurfaceFormat renderTargetViewFormat)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(!impl->hasRenderTargetViewFormats);
    impl->description.RenderTargetViewFormats.clear();
    impl->description.RenderTargetViewFormats.push_back(renderTargetViewFormat);
    impl->hasRenderTargetViewFormats = true;
    return *this;
}

Builder<PipelineState> & Builder<PipelineState>::SetRenderTargetViewFormats(
    const std::vector<SurfaceFormat>& renderTargetViewFormats)
{
    POMDOG_ASSERT(impl);
    impl->description.RenderTargetViewFormats = renderTargetViewFormats;
    impl->hasRenderTargetViewFormats = true;
    return *this;
}

Builder<PipelineState> & Builder<PipelineState>::SetRenderTargetViewFormats(
    std::vector<SurfaceFormat> && renderTargetViewFormats)
{
    POMDOG_ASSERT(impl);
    impl->description.RenderTargetViewFormats = std::move(renderTargetViewFormats);
    impl->hasRenderTargetViewFormats = true;
    return *this;
}

Builder<PipelineState> & Builder<PipelineState>::SetDepthStencilViewFormat(
    DepthFormat depthStencilViewFormat)
{
    POMDOG_ASSERT(impl);
    impl->description.DepthStencilViewFormat = depthStencilViewFormat;
    impl->hasDepthStencilViewFormat = true;
    return *this;
}

std::shared_ptr<PipelineState> Builder<PipelineState>::Build()
{
    POMDOG_ASSERT(impl);
    return impl->Load();
}

std::shared_ptr<EffectReflection> Builder<PipelineState>::CreateEffectReflection(
    const std::shared_ptr<PipelineState>& pipelineState)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(pipelineState);

    auto effectReflection = std::make_shared<EffectReflection>(
        impl->graphicsDevice,
        impl->description,
        pipelineState);
    return effectReflection;
}

const PipelineStateDescription& Builder<PipelineState>::GetDescription() const
{
    POMDOG_ASSERT(impl);
    return impl->description;
}

} // namespace AssetBuilders
} // namespace Pomdog
