// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/content/asset_builders/pipeline_state_builder.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/pipeline_descriptor.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class PipelineStateBuilder::Impl final {
public:
    gpu::PipelineDescriptor descriptor;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice;
    std::unique_ptr<Error> lastError;
    bool hasPrimitiveTopology = false;
    bool hasBlendState = false;
    bool hasRasterizerState = false;
    bool hasDepthStencilState = false;
    bool hasRenderTargetViewFormats = false;
    bool hasDepthStencilViewFormat = false;

public:
    explicit Impl(const std::shared_ptr<gpu::GraphicsDevice>& graphicsDeviceIn);

    std::tuple<std::shared_ptr<gpu::PipelineState>, std::unique_ptr<Error>>
    Load();
};

PipelineStateBuilder::Impl::Impl(const std::shared_ptr<gpu::GraphicsDevice>& graphicsDeviceIn)
    : graphicsDevice(graphicsDeviceIn)
{
    descriptor.multiSampleMask = std::numeric_limits<std::uint32_t>::max();
    descriptor.primitiveTopology = gpu::PrimitiveTopology::TriangleList;
}

std::tuple<std::shared_ptr<gpu::PipelineState>, std::unique_ptr<Error>>
PipelineStateBuilder::Impl::Load()
{
    if (lastError != nullptr) {
        return std::make_tuple(nullptr, std::move(lastError));
    }

    POMDOG_ASSERT(!descriptor.inputLayout.inputElements.empty());

    if (!hasPrimitiveTopology) {
        return std::make_tuple(nullptr, errors::make("PrimitiveTopology must be specified"));
    }

    if (!hasBlendState) {
        descriptor.blendState = gpu::BlendDescriptor::createDefault();
        hasBlendState = true;
    }

    if (!hasRasterizerState) {
        descriptor.rasterizerState = gpu::RasterizerDescriptor::createDefault();
        hasRasterizerState = true;
    }

    if (!hasDepthStencilState) {
        descriptor.depthStencilState = gpu::DepthStencilDescriptor::createDefault();
        hasDepthStencilState = true;
    }

    POMDOG_ASSERT(hasRenderTargetViewFormats);
    POMDOG_ASSERT(hasDepthStencilViewFormat);

    if (!hasRenderTargetViewFormats) {
        return std::make_tuple(nullptr, errors::make("RenderTargetViewFormats must be specified"));
    }

    if (!hasDepthStencilViewFormat) {
        return std::make_tuple(nullptr, errors::make("DepthStencilViewFormat must be specified"));
    }

    auto [pipelineState, err] = graphicsDevice->createPipelineState(descriptor);
    if (err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "createPipelineState() failed"));
    }
    return std::make_tuple(std::move(pipelineState), nullptr);
}

PipelineStateBuilder::PipelineStateBuilder(const std::shared_ptr<gpu::GraphicsDevice>& graphicsDeviceIn)
    : impl(std::make_unique<Impl>(graphicsDeviceIn))
{
}

PipelineStateBuilder::PipelineStateBuilder(PipelineStateBuilder&&) = default;
PipelineStateBuilder& PipelineStateBuilder::operator=(PipelineStateBuilder&&) = default;
PipelineStateBuilder::~PipelineStateBuilder() = default;

void PipelineStateBuilder::setVertexShader(const std::shared_ptr<gpu::Shader>& vertexShader)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(vertexShader);
    impl->descriptor.vertexShader = vertexShader;
}

void PipelineStateBuilder::setVertexShader(std::shared_ptr<gpu::Shader>&& vertexShader)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(vertexShader);
    impl->descriptor.vertexShader = std::move(vertexShader);
}

void PipelineStateBuilder::setPixelShader(const std::shared_ptr<gpu::Shader>& pixelShader)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(pixelShader);
    impl->descriptor.pixelShader = pixelShader;
}

void PipelineStateBuilder::setPixelShader(std::shared_ptr<gpu::Shader>&& pixelShader)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(pixelShader);
    impl->descriptor.pixelShader = std::move(pixelShader);
}

void PipelineStateBuilder::setInputLayout(const gpu::InputLayoutDescriptor& inputLayout)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(!inputLayout.inputElements.empty());
    impl->descriptor.inputLayout = inputLayout;
}

void PipelineStateBuilder::setInputLayout(gpu::InputLayoutDescriptor&& inputLayout)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(!inputLayout.inputElements.empty());
    impl->descriptor.inputLayout = std::move(inputLayout);
}

void PipelineStateBuilder::setPrimitiveTopology(gpu::PrimitiveTopology primitiveTopology)
{
    POMDOG_ASSERT(impl);
    impl->descriptor.primitiveTopology = primitiveTopology;
    impl->hasPrimitiveTopology = true;
}

void PipelineStateBuilder::setBlendState(const gpu::BlendDescriptor& blendState)
{
    POMDOG_ASSERT(impl);
    impl->descriptor.blendState = blendState;
    impl->hasBlendState = true;
}

void PipelineStateBuilder::setRasterizerState(const gpu::RasterizerDescriptor& rasterizerState)
{
    POMDOG_ASSERT(impl);
    impl->descriptor.rasterizerState = rasterizerState;
    impl->hasRasterizerState = true;
}

void PipelineStateBuilder::setDepthStencilState(const gpu::DepthStencilDescriptor& depthStencilState)
{
    POMDOG_ASSERT(impl);
    impl->descriptor.depthStencilState = depthStencilState;
    impl->hasDepthStencilState = true;
}

void PipelineStateBuilder::setRenderTargetViewFormat(gpu::PixelFormat renderTargetViewFormat)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(!impl->hasRenderTargetViewFormats);
    impl->descriptor.renderTargetViewFormats.clear();
    impl->descriptor.renderTargetViewFormats.push_back(renderTargetViewFormat);
    impl->hasRenderTargetViewFormats = true;
}

void PipelineStateBuilder::setRenderTargetViewFormats(const std::vector<gpu::PixelFormat>& renderTargetViewFormats)
{
    POMDOG_ASSERT(impl);
    impl->descriptor.renderTargetViewFormats = renderTargetViewFormats;
    impl->hasRenderTargetViewFormats = true;
}

void PipelineStateBuilder::setRenderTargetViewFormats(std::vector<gpu::PixelFormat>&& renderTargetViewFormats)
{
    POMDOG_ASSERT(impl);
    impl->descriptor.renderTargetViewFormats = std::move(renderTargetViewFormats);
    impl->hasRenderTargetViewFormats = true;
}

void PipelineStateBuilder::setDepthStencilViewFormat(gpu::PixelFormat depthStencilViewFormat)
{
    POMDOG_ASSERT(impl);
    impl->descriptor.depthStencilViewFormat = depthStencilViewFormat;
    impl->hasDepthStencilViewFormat = true;
}

std::tuple<std::shared_ptr<gpu::PipelineState>, std::unique_ptr<Error>>
PipelineStateBuilder::build()
{
    POMDOG_ASSERT(impl);
    return impl->Load();
}

const gpu::PipelineDescriptor&
PipelineStateBuilder::getDescription() const
{
    POMDOG_ASSERT(impl);
    return impl->descriptor;
}

void PipelineStateBuilder::setError(std::unique_ptr<Error>&& err)
{
    POMDOG_ASSERT(impl != nullptr);
    POMDOG_ASSERT(impl->lastError != nullptr);
    impl->lastError = std::move(err);
}

} // namespace pomdog
