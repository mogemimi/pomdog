// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/image_effects/chromatic_aberration.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/asset_builders/pipeline_state_builder.h"
#include "pomdog/content/shader_loader.h"
#include "pomdog/gpu/blend_desc.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/depth_stencil_desc.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/input_layout_helper.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/primitive_topology.h"
#include "pomdog/gpu/render_target2d.h"
#include "pomdog/gpu/sampler_state.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

namespace pomdog {

std::unique_ptr<Error>
ChromaticAberration::initialize(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice)
{
    auto [sampler, samplerErr] = graphicsDevice->createSamplerState(
        gpu::SamplerDesc::createLinearClamp());
    if (samplerErr != nullptr) {
        return errors::wrap(std::move(samplerErr), "failed to create sampler state");
    }
    samplerState_ = std::move(sampler);

    auto inputLayout = gpu::InputLayoutHelper{}
                           .addFloat3()
                           .addFloat2();

    auto [vertexShader, vsErr] = loadShaderAutomagically(
        fs, graphicsDevice,
        gpu::ShaderPipelineStage::VertexShader,
        "/assets/shaders", "screen_quad_vs", "screen_quad_vs");
    if (vsErr != nullptr) {
        return errors::wrap(std::move(vsErr), "failed to load vertex shader");
    }

    auto [pixelShader, psErr] = loadShaderAutomagically(
        fs, graphicsDevice,
        gpu::ShaderPipelineStage::PixelShader,
        "/assets/shaders", "chromatic_aberration_ps", "chromatic_aberration_ps");
    if (psErr != nullptr) {
        return errors::wrap(std::move(psErr), "failed to load pixel shader");
    }

    auto presentationParameters = graphicsDevice->getPresentationParameters();

    auto pipelineStateBuilder = PipelineStateBuilder(graphicsDevice);
    pipelineStateBuilder.setRenderTargetViewFormat(presentationParameters.backBufferFormat);
    pipelineStateBuilder.setDepthStencilViewFormat(presentationParameters.depthStencilFormat);
    pipelineStateBuilder.setVertexShader(std::move(vertexShader));
    pipelineStateBuilder.setPixelShader(std::move(pixelShader));
    pipelineStateBuilder.setInputLayout(inputLayout.createInputLayout());
    pipelineStateBuilder.setPrimitiveTopology(gpu::PrimitiveTopology::TriangleList);
    pipelineStateBuilder.setBlendState(gpu::BlendDesc::createOpaque());
    pipelineStateBuilder.setDepthStencilState(gpu::DepthStencilDesc::createNone());

    auto [pipeline, pipelineErr] = pipelineStateBuilder.build();
    if (pipelineErr != nullptr) {
        return errors::wrap(std::move(pipelineErr), "failed to create pipeline state");
    }
    pipelineState_ = std::move(pipeline);

    return nullptr;
}

void ChromaticAberration::updateGPUResources()
{
}

void ChromaticAberration::apply(
    gpu::CommandList& commandList,
    const std::shared_ptr<gpu::RenderTarget2D>& source,
    const std::shared_ptr<gpu::ConstantBuffer>& constantBuffer)
{
    POMDOG_ASSERT(source);
    POMDOG_ASSERT(constantBuffer);
    commandList.setConstantBuffer(0, constantBuffer);
    commandList.setSamplerState(0, samplerState_);
    commandList.setTexture(0, source);
    commandList.setPipelineState(pipelineState_);
}

} // namespace pomdog
