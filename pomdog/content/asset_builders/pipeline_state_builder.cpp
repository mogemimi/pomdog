// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/content/asset_builders/pipeline_state_builder.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/asset_manager.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/pipeline_descriptor.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/gpu/shader_reflections/effect_reflection.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::AssetBuilders {

class Builder<gpu::PipelineState>::Impl final {
public:
    gpu::PipelineDescriptor descriptor;
    std::reference_wrapper<AssetManager const> assets;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice;
    std::unique_ptr<Error> lastError;
    bool hasPrimitiveTopology = false;
    bool hasBlendState = false;
    bool hasRasterizerState = false;
    bool hasDepthStencilState = false;
    bool hasRenderTargetViewFormats = false;
    bool hasDepthStencilViewFormat = false;

public:
    explicit Impl(AssetManager& assets);

    std::tuple<std::shared_ptr<gpu::PipelineState>, std::unique_ptr<Error>>
    Load();
};

Builder<gpu::PipelineState>::Impl::Impl(AssetManager& assetsIn)
    : assets(assetsIn)
{
    graphicsDevice = assetsIn.GetGraphicsDevice();
    descriptor.MultiSampleMask = std::numeric_limits<std::uint32_t>::max();
    descriptor.PrimitiveTopology = gpu::PrimitiveTopology::TriangleList;
}

std::tuple<std::shared_ptr<gpu::PipelineState>, std::unique_ptr<Error>>
Builder<gpu::PipelineState>::Impl::Load()
{
    if (lastError != nullptr) {
        return std::make_tuple(nullptr, std::move(lastError));
    }

    POMDOG_ASSERT(!descriptor.InputLayout.InputElements.empty());

    if (!hasPrimitiveTopology) {
        return std::make_tuple(nullptr, errors::New("PrimitiveTopology must be specified"));
    }

    if (!hasBlendState) {
        descriptor.BlendState = gpu::BlendDescriptor::CreateDefault();
        hasBlendState = true;
    }

    if (!hasRasterizerState) {
        descriptor.RasterizerState = gpu::RasterizerDescriptor::CreateDefault();
        hasRasterizerState = true;
    }

    if (!hasDepthStencilState) {
        descriptor.DepthStencilState = gpu::DepthStencilDescriptor::CreateDefault();
        hasDepthStencilState = true;
    }

    POMDOG_ASSERT(hasRenderTargetViewFormats);
    POMDOG_ASSERT(hasDepthStencilViewFormat);

    if (!hasRenderTargetViewFormats) {
        return std::make_tuple(nullptr, errors::New("RenderTargetViewFormats must be specified"));
    }

    if (!hasDepthStencilViewFormat) {
        return std::make_tuple(nullptr, errors::New("DepthStencilViewFormat must be specified"));
    }

    auto [pipelineState, err] = graphicsDevice->CreatePipelineState(descriptor);
    if (err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "CreatePipelineState() failed"));
    }
    return std::make_tuple(std::move(pipelineState), nullptr);
}

Builder<gpu::PipelineState>::Builder(AssetManager& assetsIn)
    : impl(std::make_unique<Impl>(assetsIn))
{
}

Builder<gpu::PipelineState>::Builder(Builder<gpu::PipelineState>&&) = default;
Builder<gpu::PipelineState>& Builder<gpu::PipelineState>::operator=(Builder<gpu::PipelineState>&&) = default;
Builder<gpu::PipelineState>::~Builder() = default;

Builder<gpu::PipelineState>&
Builder<gpu::PipelineState>::SetVertexShader(const std::shared_ptr<gpu::Shader>& vertexShader)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(vertexShader);
    impl->descriptor.VertexShader = vertexShader;
    return *this;
}

Builder<gpu::PipelineState>&
Builder<gpu::PipelineState>::SetVertexShader(std::shared_ptr<gpu::Shader>&& vertexShader)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(vertexShader);
    impl->descriptor.VertexShader = std::move(vertexShader);
    return *this;
}

Builder<gpu::PipelineState>&
Builder<gpu::PipelineState>::SetPixelShader(const std::shared_ptr<gpu::Shader>& pixelShader)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(pixelShader);
    impl->descriptor.PixelShader = pixelShader;
    return *this;
}

Builder<gpu::PipelineState>&
Builder<gpu::PipelineState>::SetPixelShader(std::shared_ptr<gpu::Shader>&& pixelShader)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(pixelShader);
    impl->descriptor.PixelShader = std::move(pixelShader);
    return *this;
}

Builder<gpu::PipelineState>&
Builder<gpu::PipelineState>::SetInputLayout(const gpu::InputLayoutDescriptor& inputLayout)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(!inputLayout.InputElements.empty());
    impl->descriptor.InputLayout = inputLayout;
    return *this;
}

Builder<gpu::PipelineState>&
Builder<gpu::PipelineState>::SetInputLayout(gpu::InputLayoutDescriptor&& inputLayout)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(!inputLayout.InputElements.empty());
    impl->descriptor.InputLayout = std::move(inputLayout);
    return *this;
}

Builder<gpu::PipelineState>&
Builder<gpu::PipelineState>::SetPrimitiveTopology(gpu::PrimitiveTopology primitiveTopology)
{
    POMDOG_ASSERT(impl);
    impl->descriptor.PrimitiveTopology = primitiveTopology;
    impl->hasPrimitiveTopology = true;
    return *this;
}

Builder<gpu::PipelineState>&
Builder<gpu::PipelineState>::SetBlendState(const gpu::BlendDescriptor& blendState)
{
    POMDOG_ASSERT(impl);
    impl->descriptor.BlendState = blendState;
    impl->hasBlendState = true;
    return *this;
}

Builder<gpu::PipelineState>&
Builder<gpu::PipelineState>::SetRasterizerState(const gpu::RasterizerDescriptor& rasterizerState)
{
    POMDOG_ASSERT(impl);
    impl->descriptor.RasterizerState = rasterizerState;
    impl->hasRasterizerState = true;
    return *this;
}

Builder<gpu::PipelineState>&
Builder<gpu::PipelineState>::SetDepthStencilState(const gpu::DepthStencilDescriptor& depthStencilState)
{
    POMDOG_ASSERT(impl);
    impl->descriptor.DepthStencilState = depthStencilState;
    impl->hasDepthStencilState = true;
    return *this;
}

Builder<gpu::PipelineState>&
Builder<gpu::PipelineState>::SetConstantBufferBindSlot(const std::string& name, int slotIndex)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(!name.empty());
    POMDOG_ASSERT(slotIndex >= 0);

#if defined(DEBUG) && !defined(NDEBUG)
    for (auto& pair : impl->descriptor.ConstantBufferBindHints) {
        POMDOG_ASSERT(slotIndex != pair.second);
    }
#endif

    impl->descriptor.ConstantBufferBindHints.emplace(name, slotIndex);
    return *this;
}

Builder<gpu::PipelineState>&
Builder<gpu::PipelineState>::SetSamplerBindSlot(const std::string& name, int slotIndex)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(!name.empty());
    POMDOG_ASSERT(slotIndex >= 0);

#if defined(DEBUG) && !defined(NDEBUG)
    for (auto& pair : impl->descriptor.SamplerBindHints) {
        POMDOG_ASSERT(slotIndex != pair.second);
    }
#endif

    impl->descriptor.SamplerBindHints.emplace(name, slotIndex);
    return *this;
}

Builder<gpu::PipelineState>&
Builder<gpu::PipelineState>::SetRenderTargetViewFormat(PixelFormat renderTargetViewFormat)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(!impl->hasRenderTargetViewFormats);
    impl->descriptor.RenderTargetViewFormats.clear();
    impl->descriptor.RenderTargetViewFormats.push_back(renderTargetViewFormat);
    impl->hasRenderTargetViewFormats = true;
    return *this;
}

Builder<gpu::PipelineState>&
Builder<gpu::PipelineState>::SetRenderTargetViewFormats(const std::vector<PixelFormat>& renderTargetViewFormats)
{
    POMDOG_ASSERT(impl);
    impl->descriptor.RenderTargetViewFormats = renderTargetViewFormats;
    impl->hasRenderTargetViewFormats = true;
    return *this;
}

Builder<gpu::PipelineState>&
Builder<gpu::PipelineState>::SetRenderTargetViewFormats(std::vector<PixelFormat>&& renderTargetViewFormats)
{
    POMDOG_ASSERT(impl);
    impl->descriptor.RenderTargetViewFormats = std::move(renderTargetViewFormats);
    impl->hasRenderTargetViewFormats = true;
    return *this;
}

Builder<gpu::PipelineState>&
Builder<gpu::PipelineState>::SetDepthStencilViewFormat(PixelFormat depthStencilViewFormat)
{
    POMDOG_ASSERT(impl);
    impl->descriptor.DepthStencilViewFormat = depthStencilViewFormat;
    impl->hasDepthStencilViewFormat = true;
    return *this;
}

std::tuple<std::shared_ptr<gpu::PipelineState>, std::unique_ptr<Error>>
Builder<gpu::PipelineState>::Build()
{
    POMDOG_ASSERT(impl);
    return impl->Load();
}

std::shared_ptr<gpu::EffectReflection>
Builder<gpu::PipelineState>::CreateEffectReflection(const std::shared_ptr<gpu::PipelineState>& pipelineState)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(pipelineState);

    auto effectReflection = std::get<0>(impl->graphicsDevice->CreateEffectReflection(
        impl->descriptor,
        pipelineState));
    return effectReflection;
}

const gpu::PipelineDescriptor& Builder<gpu::PipelineState>::GetDescription() const
{
    POMDOG_ASSERT(impl);
    return impl->descriptor;
}

void Builder<gpu::PipelineState>::SetError(std::unique_ptr<Error>&& err)
{
    POMDOG_ASSERT(impl != nullptr);
    POMDOG_ASSERT(impl->lastError != nullptr);
    impl->lastError = std::move(err);
}

} // namespace pomdog::AssetBuilders
