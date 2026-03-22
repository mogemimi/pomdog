// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/basic_effect.h"
#include "pomdog/content/asset_builders/pipeline_state_builder.h"
#include "pomdog/content/shader_loader.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/input_layout_helper.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/gpu/shader_pipeline_stage.h"
#include "pomdog/utility/errors.h"

namespace pomdog::BasicEffect {

[[nodiscard]] std::tuple<PipelineStateBuilder, std::unique_ptr<Error>>
createBasicEffect(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    BasicEffectVariant variant)
{
    gpu::InputLayoutHelper inputLayoutBuilder;

    std::string vsName;
    std::string psName;

    switch (variant) {
    case BasicEffectVariant::PositionColor:
        inputLayoutBuilder.addFloat3(); // position
        inputLayoutBuilder.addFloat4(); // color
        vsName = "basic_effect_position_color_vs";
        psName = "basic_effect_position_color_ps";
        break;
    case BasicEffectVariant::PositionTexture:
        inputLayoutBuilder.addFloat3(); // position
        inputLayoutBuilder.addFloat2(); // texcoord
        vsName = "basic_effect_position_texture_vs";
        psName = "basic_effect_position_texture_ps";
        break;
    case BasicEffectVariant::PositionNormalColor:
        inputLayoutBuilder.addFloat3(); // position
        inputLayoutBuilder.addFloat3(); // normal
        inputLayoutBuilder.addFloat4(); // color
        vsName = "basic_effect_position_normal_color_vs";
        psName = "basic_effect_position_normal_color_ps";
        break;
    case BasicEffectVariant::PositionNormalTexture:
        inputLayoutBuilder.addFloat3(); // position
        inputLayoutBuilder.addFloat3(); // normal
        inputLayoutBuilder.addFloat2(); // texcoord
        vsName = "basic_effect_position_normal_texture_vs";
        psName = "basic_effect_position_normal_texture_ps";
        break;
    }

    auto inputLayout = inputLayoutBuilder.createInputLayout();

    auto [vertexShader, vsErr] = loadShaderAutomagically(
        fs, graphicsDevice,
        gpu::ShaderPipelineStage::VertexShader,
        "/assets/shaders", vsName, vsName);
    if (vsErr != nullptr) {
        return {PipelineStateBuilder(graphicsDevice), errors::wrap(std::move(vsErr), "failed to load vertex shader")};
    }

    auto [pixelShader, psErr] = loadShaderAutomagically(
        fs, graphicsDevice,
        gpu::ShaderPipelineStage::PixelShader,
        "/assets/shaders", psName, psName);
    if (psErr != nullptr) {
        return {PipelineStateBuilder(graphicsDevice), errors::wrap(std::move(psErr), "failed to load pixel shader")};
    }

    auto builder = PipelineStateBuilder(graphicsDevice);
    builder.setInputLayout(inputLayout);
    builder.setVertexShader(std::move(vertexShader));
    builder.setPixelShader(std::move(pixelShader));

    return {std::move(builder), nullptr};
}

} // namespace pomdog::BasicEffect
