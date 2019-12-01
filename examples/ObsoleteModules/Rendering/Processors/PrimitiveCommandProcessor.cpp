// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "PrimitiveCommandProcessor.hpp"
#include "Pomdog.Experimental/Rendering/Commands/PrimitiveCommand.hpp"
#include <cstring>

namespace Pomdog {
namespace Rendering {
namespace {

// Built-in shaders
// FIXME: Bad include paths
#include "../../../../src/Experimental/Graphics/Shaders/GLSL.Embedded/PrimitiveBatch_VS.inc.hpp"
#include "../../../../src/Experimental/Graphics/Shaders/GLSL.Embedded/PrimitiveBatch_PS.inc.hpp"
#include "../../../../src/Experimental/Graphics/Shaders/HLSL.Embedded/PrimitiveBatch_VS.inc.hpp"
#include "../../../../src/Experimental/Graphics/Shaders/HLSL.Embedded/PrimitiveBatch_PS.inc.hpp"
#include "../../../../src/Experimental/Graphics/Shaders/Metal.Embedded/PrimitiveBatch.inc.hpp"

} // unnamed namespace

PrimitiveCommandProcessor::PrimitiveCommandProcessor(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    AssetManager& assets)
    : drawCallCount(0)
{
    auto inputLayout = InputLayoutHelper{}
        .Float3().Float4();

    auto vertexShader = assets.CreateBuilder<Shader>(ShaderPipelineStage::VertexShader)
        .SetGLSL(Builtin_GLSL_PrimitiveBatch_VS, std::strlen(Builtin_GLSL_PrimitiveBatch_VS))
        .SetHLSLPrecompiled(BuiltinHLSL_PrimitiveBatch_VS, sizeof(BuiltinHLSL_PrimitiveBatch_VS))
        .SetMetal(Builtin_Metal_PrimitiveBatch, std::strlen(Builtin_Metal_PrimitiveBatch), "PrimitiveBatchVS");

    auto pixelShader = assets.CreateBuilder<Shader>(ShaderPipelineStage::PixelShader)
        .SetGLSL(Builtin_GLSL_PrimitiveBatch_PS, std::strlen(Builtin_GLSL_PrimitiveBatch_PS))
        .SetHLSLPrecompiled(BuiltinHLSL_PrimitiveBatch_PS, sizeof(BuiltinHLSL_PrimitiveBatch_PS))
        .SetMetal(Builtin_Metal_PrimitiveBatch, std::strlen(Builtin_Metal_PrimitiveBatch), "PrimitiveBatchPS");

    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    pipelineState = assets.CreateBuilder<PipelineState>()
        .SetRenderTargetViewFormat(presentationParameters.BackBufferFormat)
        .SetDepthStencilViewFormat(presentationParameters.DepthStencilFormat)
        .SetVertexShader(vertexShader.Build())
        .SetPixelShader(pixelShader.Build())
        .SetInputLayout(inputLayout.CreateInputLayout())
        .SetBlendState(BlendDescription::CreateNonPremultiplied())
        .SetDepthStencilState(DepthStencilDescription::CreateReadWriteDepth())
        .SetConstantBufferBindSlot("TransformMatrix", 0)
        .Build();
}

void PrimitiveCommandProcessor::Begin(
    const std::shared_ptr<GraphicsCommandList>& commandList,
    const Matrix4x4& viewProjectionIn)
{
    drawCallCount = 0;
    viewProjection = viewProjectionIn;
}

void PrimitiveCommandProcessor::Draw(
    const std::shared_ptr<GraphicsCommandList>& commandList,
    RenderCommand& command)
{
    using Pomdog::Rendering::PrimitiveCommand;
    auto& primitiveCommand = static_cast<PrimitiveCommand&>(command);
    POMDOG_ASSERT(primitiveCommand.vertexBuffer);
    POMDOG_ASSERT(primitiveCommand.constantBuffer);

    POMDOG_ASSERT(primitiveCommand.constantBuffer);
    POMDOG_ASSERT(primitiveCommand.constantBuffer->GetSizeInBytes() >= sizeof(Matrix4x4));
    alignas(16) Matrix4x4 transposed = Matrix4x4::Transpose(primitiveCommand.worldMatrix * viewProjection);
    primitiveCommand.constantBuffer->SetValue(transposed);

    POMDOG_ASSERT(commandList);
    POMDOG_ASSERT(primitiveCommand.vertexBuffer->GetVertexCount() > 0);
    POMDOG_ASSERT(primitiveCommand.vertexCount > 0);
    POMDOG_ASSERT(primitiveCommand.vertexCount <= primitiveCommand.vertexBuffer->GetVertexCount());

    commandList->SetPipelineState(pipelineState);
    commandList->SetPrimitiveTopology(PrimitiveTopology::TriangleList);
    commandList->SetVertexBuffer(0, primitiveCommand.vertexBuffer);
    commandList->SetConstantBuffer(0, primitiveCommand.constantBuffer);
    commandList->Draw(primitiveCommand.vertexCount, 0);

    ++drawCallCount;
}

void PrimitiveCommandProcessor::End()
{
}

void PrimitiveCommandProcessor::FlushBatch()
{
}

int PrimitiveCommandProcessor::GetDrawCallCount() const noexcept
{
    return drawCallCount;
}

std::type_index PrimitiveCommandProcessor::GetCommandType() const noexcept
{
    std::type_index index = typeid(PrimitiveCommand);
    return index;
}

} // namespace Rendering
} // namespace Pomdog
