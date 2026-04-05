// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/image_effects/retro_crt_effect.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/shader_loader.h"
#include "pomdog/gpu/blend_desc.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/depth_stencil_desc.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/input_layout_helper.h"
#include "pomdog/gpu/pipeline_desc.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/primitive_topology.h"
#include "pomdog/gpu/render_target2d.h"
#include "pomdog/gpu/sampler_state.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

std::unique_ptr<Error>
RetroCrtEffect::initialize(
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
        "/assets/shaders", "retro_crt_effect_ps", "retro_crt_effect_ps");
    if (psErr != nullptr) {
        return errors::wrap(std::move(psErr), "failed to load pixel shader");
    }

    auto presentationParameters = graphicsDevice->getPresentationParameters();

    gpu::PipelineDesc pipelineDesc = {};
    pipelineDesc.renderTargetViewFormats = {presentationParameters.backBufferFormat};
    pipelineDesc.depthStencilViewFormat = presentationParameters.depthStencilFormat;
    pipelineDesc.vertexShader = std::move(vertexShader);
    pipelineDesc.pixelShader = std::move(pixelShader);
    pipelineDesc.inputLayout = inputLayout.createInputLayout();
    pipelineDesc.primitiveTopology = gpu::PrimitiveTopology::TriangleList;
    pipelineDesc.blendState = gpu::BlendDesc::createOpaque();
    pipelineDesc.depthStencilState = gpu::DepthStencilDesc::createNone();
    pipelineDesc.rasterizerState = gpu::RasterizerDesc::createDefault();
    pipelineDesc.multiSampleMask = std::numeric_limits<u32>::max();

    auto [pipeline, pipelineErr] = graphicsDevice->createPipelineState(pipelineDesc);
    if (pipelineErr != nullptr) {
        return errors::wrap(std::move(pipelineErr), "failed to create pipeline state");
    }
    pipelineState_ = std::move(pipeline);

    return nullptr;
}

void RetroCrtEffect::updateGPUResources()
{
}

void RetroCrtEffect::apply(
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
