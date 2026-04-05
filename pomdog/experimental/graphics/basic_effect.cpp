// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/basic_effect.h"
#include "pomdog/content/shader_loader.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/input_layout_builder.h"
#include "pomdog/gpu/input_layout_desc.h"
#include "pomdog/gpu/pipeline_desc.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/gpu/shader_pipeline_stage.h"
#include "pomdog/utility/errors.h"

namespace pomdog::BasicEffect {

[[nodiscard]] std::tuple<gpu::PipelineDesc, std::unique_ptr<Error>>
createBasicEffect(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    BasicEffectVariant variant)
{
    std::string vsName;
    std::string psName;

    gpu::InputLayoutDesc inputLayout = {};

    switch (variant) {
    case BasicEffectVariant::PositionColor:
        gpu::InputLayoutBuilder::addVertex(inputLayout,
            0, gpu::InputClassification::PerVertex, 28,
            {
                gpu::InputElementFormat::Float32x3,
                gpu::InputElementFormat::Float32x4,
            });
        vsName = "basic_effect_position_color_vs";
        psName = "basic_effect_position_color_ps";
        break;
    case BasicEffectVariant::PositionTexture:
        gpu::InputLayoutBuilder::addVertex(inputLayout,
            0, gpu::InputClassification::PerVertex, 20,
            {
                gpu::InputElementFormat::Float32x3,
                gpu::InputElementFormat::Float32x2,
            });
        vsName = "basic_effect_position_texture_vs";
        psName = "basic_effect_position_texture_ps";
        break;
    case BasicEffectVariant::PositionNormalColor:
        gpu::InputLayoutBuilder::addVertex(inputLayout,
            0, gpu::InputClassification::PerVertex, 40,
            {
                gpu::InputElementFormat::Float32x3,
                gpu::InputElementFormat::Float32x3,
                gpu::InputElementFormat::Float32x4,
            });
        vsName = "basic_effect_position_normal_color_vs";
        psName = "basic_effect_position_normal_color_ps";
        break;
    case BasicEffectVariant::PositionNormalTexture:
        gpu::InputLayoutBuilder::addVertex(inputLayout,
            0, gpu::InputClassification::PerVertex, 32,
            {
                gpu::InputElementFormat::Float32x3,
                gpu::InputElementFormat::Float32x3,
                gpu::InputElementFormat::Float32x2,
            });
        vsName = "basic_effect_position_normal_texture_vs";
        psName = "basic_effect_position_normal_texture_ps";
        break;
    }

    auto [vertexShader, vsErr] = loadShaderAutomagically(
        fs, graphicsDevice,
        gpu::ShaderPipelineStage::VertexShader,
        "/assets/shaders", vsName, vsName);
    if (vsErr != nullptr) {
        return {gpu::PipelineDesc{}, errors::wrap(std::move(vsErr), "failed to load vertex shader")};
    }

    auto [pixelShader, psErr] = loadShaderAutomagically(
        fs, graphicsDevice,
        gpu::ShaderPipelineStage::PixelShader,
        "/assets/shaders", psName, psName);
    if (psErr != nullptr) {
        return {gpu::PipelineDesc{}, errors::wrap(std::move(psErr), "failed to load pixel shader")};
    }

    gpu::PipelineDesc desc = {};
    desc.inputLayout = inputLayout;
    desc.vertexShader = std::move(vertexShader);
    desc.pixelShader = std::move(pixelShader);

    return {std::move(desc), nullptr};
}

} // namespace pomdog::BasicEffect
