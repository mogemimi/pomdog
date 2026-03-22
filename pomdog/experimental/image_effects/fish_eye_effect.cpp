// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/image_effects/fish_eye_effect.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/asset_builders/pipeline_state_builder.h"
#include "pomdog/content/shader_loader.h"
#include "pomdog/gpu/blend_descriptor.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/depth_stencil_descriptor.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/input_layout_helper.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/primitive_topology.h"
#include "pomdog/gpu/render_target2d.h"
#include "pomdog/gpu/sampler_state.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

namespace pomdog {
namespace {

struct FishEyeBlock final {
    float strength;
    float x[15];
};

} // namespace

std::unique_ptr<Error>
FishEyeEffect::initialize(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice)
{
    auto [sampler, samplerErr] = graphicsDevice->createSamplerState(
        gpu::SamplerDescriptor::createLinearWrap());
    if (samplerErr != nullptr) {
        return errors::wrap(std::move(samplerErr), "failed to create sampler state");
    }
    samplerLinear_ = std::move(sampler);

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
        "/assets/shaders", "fish_eye_ps", "fish_eye_ps");
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
    pipelineStateBuilder.setBlendState(gpu::BlendDescriptor::createOpaque());
    pipelineStateBuilder.setDepthStencilState(gpu::DepthStencilDescriptor::createNone());

    auto [pipeline, pipelineErr] = pipelineStateBuilder.build();
    if (pipelineErr != nullptr) {
        return errors::wrap(std::move(pipelineErr), "failed to create pipeline state");
    }
    pipelineState_ = std::move(pipeline);

    auto [cbuffer, cbufferErr] = graphicsDevice->createConstantBuffer(
        sizeof(FishEyeBlock),
        gpu::BufferUsage::Dynamic);
    if (cbufferErr != nullptr) {
        return errors::wrap(std::move(cbufferErr), "failed to create constant buffer");
    }
    constantBufferFishEye_ = std::move(cbuffer);

    setStrength(0.04f);

    return nullptr;
}

void FishEyeEffect::setStrength(float strength)
{
    strength_ = math::clamp(strength, -0.5f, 0.5f);
}

void FishEyeEffect::updateGPUResources()
{
    FishEyeBlock block;
    block.strength = strength_;

    POMDOG_ASSERT(constantBufferFishEye_);
    constantBufferFishEye_->setData(0, gpu::makeByteSpan(block));
}

void FishEyeEffect::apply(
    gpu::CommandList& commandList,
    const std::shared_ptr<gpu::RenderTarget2D>& source,
    const std::shared_ptr<gpu::ConstantBuffer>& constantBuffer)
{
    POMDOG_ASSERT(source);
    POMDOG_ASSERT(constantBuffer);
    commandList.setConstantBuffer(0, constantBuffer);
    commandList.setConstantBuffer(1, constantBufferFishEye_);
    commandList.setSamplerState(0, samplerLinear_);
    commandList.setTexture(0, source);
    commandList.setPipelineState(pipelineState_);
}

} // namespace pomdog
