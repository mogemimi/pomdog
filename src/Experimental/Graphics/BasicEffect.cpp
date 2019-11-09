// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Graphics/BasicEffect.hpp"
#include "Pomdog/Content/AssetBuilders/PipelineStateBuilder.hpp"
#include "Pomdog/Content/AssetBuilders/ShaderBuilder.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/InputLayoutHelper.hpp"
#include "Pomdog/Graphics/PipelineState.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Graphics/ShaderPipelineStage.hpp"

namespace Pomdog::BasicEffect {
namespace {

// Built-in shaders
#include "Shaders/GLSL.Embedded/BasicEffect_PS.inc.hpp"
#include "Shaders/GLSL.Embedded/BasicEffect_VS.inc.hpp"
#include "Shaders/HLSL.Embedded/BasicEffect.inc.hpp"
#include "Shaders/Metal.Embedded/BasicEffect.inc.hpp"

} // namespace

AssetBuilders::Builder<PipelineState>
CreateBasicEffect(AssetManager& assets, const BasicEffectDescription& desc)
{
    InputLayoutHelper inputLayoutBuilder;
    inputLayoutBuilder.Float3();

    if (desc.LightingEnabled) {
        // NOTE: Add surface normal into the input layout.
        inputLayoutBuilder.Float3();
    }

    if (desc.TextureEnabled) {
        // NOTE: Add texture coordinates into the input layout.
        inputLayoutBuilder.Float2();
    }
    else {
        // NOTE: Add vertex color into the input layout.
        inputLayoutBuilder.Float4();
    }

    // NOTE: Generate shader code for Metal
    std::string preprocessorMacroLines;
    if (desc.LightingEnabled) {
        preprocessorMacroLines += "#define LIGHTING_ENABLED 1\n";
    }
    else {
        preprocessorMacroLines += "#define LIGHTING_ENABLED 0\n";
    }
    if (desc.TextureEnabled) {
        preprocessorMacroLines += "#define TEXTURE_ENABLED 1\n";
        preprocessorMacroLines += "#define VERTEX_COLOR_ENABLED 0\n";
    }
    else {
        preprocessorMacroLines += "#define TEXTURE_ENABLED 0\n";
        preprocessorMacroLines += "#define VERTEX_COLOR_ENABLED 1\n";
    }

    std::string sourceHLSL = preprocessorMacroLines + BuiltinHLSL_BasicEffect;
    std::string sourceMetal = preprocessorMacroLines + Builtin_Metal_BasicEffect;
    std::string sourceGLSLVS = "#version 330\n" + preprocessorMacroLines + Builtin_GLSL_BasicEffect_VS;
    std::string sourceGLSLPS = "#version 330\n" + preprocessorMacroLines + Builtin_GLSL_BasicEffect_PS;

    auto inputLayout = inputLayoutBuilder.CreateInputLayout();

    auto vertexShaderBuilder = assets.CreateBuilder<Shader>(ShaderPipelineStage::VertexShader);

    auto vertexShader = vertexShaderBuilder
        .SetGLSL(sourceGLSLVS.data(), sourceGLSLVS.size())
        .SetHLSL(sourceHLSL.data(), sourceHLSL.size(), "BasicEffectVS")
        .SetMetal(sourceMetal.data(), sourceMetal.size(), "BasicEffectVS")
        .Build();

    auto pixelShaderBuilder = assets.CreateBuilder<Shader>(ShaderPipelineStage::PixelShader);

    auto pixelShader = pixelShaderBuilder
        .SetGLSL(sourceGLSLPS.data(), sourceGLSLPS.size())
        .SetHLSL(sourceHLSL.data(), sourceHLSL.size(), "BasicEffectPS")
        .SetMetal(sourceMetal.data(), sourceMetal.size(), "BasicEffectPS")
        .Build();

    auto graphicsDevice = assets.GetGraphicsDevice();

    auto builder = assets.CreateBuilder<PipelineState>();
    builder.SetInputLayout(inputLayout);
    builder.SetVertexShader(std::move(vertexShader));
    builder.SetPixelShader(std::move(pixelShader));
    builder.SetConstantBufferBindSlot("ModelConstantBuffer", 0);
    builder.SetConstantBufferBindSlot("WorldConstantBuffer", 1);

    return builder;
}

} // namespace Pomdog::BasicEffect
