// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/wireframe_effect.h"
#include "pomdog/content/shader_loader.h"
#include "pomdog/gpu/blend_desc.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/depth_stencil_desc.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/input_layout_builder.h"
#include "pomdog/gpu/input_layout_desc.h"
#include "pomdog/gpu/pipeline_desc.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/primitive_topology.h"
#include "pomdog/gpu/rasterizer_desc.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/gpu/shader_pipeline_stage.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/math/color.h"
#include "pomdog/math/vector3.h"
#include "pomdog/math/vector4.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

struct VertexPositionColor final {
    Vector3 position;
    Vector4 color;
};

static_assert(sizeof(VertexPositionColor) == 28);

} // namespace

WireframeEffect::WireframeEffect() = default;

std::unique_ptr<Error>
WireframeEffect::initialize(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice)
{
    auto presentationParameters = graphicsDevice->getPresentationParameters();

    auto [vertexShader, vsErr] = loadShaderAutomagically(
        fs, graphicsDevice,
        gpu::ShaderPipelineStage::VertexShader,
        "/assets/shaders",
        "basic_effect_position_color_vs",
        "basic_effect_position_color_vs");
    if (vsErr != nullptr) {
        return errors::wrap(std::move(vsErr), "failed to load vertex shader for WireframeEffect");
    }

    auto [pixelShader, psErr] = loadShaderAutomagically(
        fs, graphicsDevice,
        gpu::ShaderPipelineStage::PixelShader,
        "/assets/shaders",
        "basic_effect_position_color_ps",
        "basic_effect_position_color_ps");
    if (psErr != nullptr) {
        return errors::wrap(std::move(psErr), "failed to load pixel shader for WireframeEffect");
    }

    gpu::PipelineDesc pipelineDesc = {};
    pipelineDesc.renderTargetViewFormats = {presentationParameters.backBufferFormat};
    pipelineDesc.depthStencilViewFormat = presentationParameters.depthStencilFormat;
    pipelineDesc.vertexShader = std::move(vertexShader);
    pipelineDesc.pixelShader = std::move(pixelShader);
    gpu::InputLayoutBuilder::addVertex(pipelineDesc.inputLayout,
        0, gpu::InputClassification::PerVertex, 28,
        {
            gpu::InputElementFormat::Float32x3,
            gpu::InputElementFormat::Float32x4,
        });
    pipelineDesc.primitiveTopology = gpu::PrimitiveTopology::LineList;
    pipelineDesc.blendState = gpu::BlendDesc::createAlphaBlend();
    pipelineDesc.rasterizerState = gpu::RasterizerDesc::createCullNone();
    pipelineDesc.depthStencilState = gpu::DepthStencilDesc::createDefault();
    pipelineDesc.multiSampleMask = std::numeric_limits<u32>::max();

    auto [ps, psStateErr] = graphicsDevice->createPipelineState(pipelineDesc);
    if (psStateErr != nullptr) {
        return errors::wrap(std::move(psStateErr), "failed to create pipeline state for WireframeEffect");
    }
    pipelineState_ = std::move(ps);

    return nullptr;
}

void WireframeEffect::setTriangleMesh(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const Color& color,
    u32 indexCount,
    std::function<Vector3(u32 index)> getVertex)
{
    POMDOG_ASSERT(graphicsDevice != nullptr);
    POMDOG_ASSERT(getVertex != nullptr);
    POMDOG_ASSERT(indexCount % 3 == 0);

    // Each triangle produces 3 line segments (6 vertices).
    const u32 newVertexCount = indexCount * 2;

    if (newVertexCount == 0) {
        vertexCount_ = 0;
        return;
    }

    const Vector4 colorVec = color.toVector4();

    std::vector<VertexPositionColor> lineVertices = {};
    lineVertices.reserve(newVertexCount);

    for (u32 t = 0; t + 2 < indexCount; t += 3) {
        const auto p0 = getVertex(t);
        const auto p1 = getVertex(t + 1);
        const auto p2 = getVertex(t + 2);

        // NOTE: Edge 0 -> 1
        lineVertices.push_back({p0, colorVec});
        lineVertices.push_back({p1, colorVec});

        // NOTE: Edge 1 -> 2
        lineVertices.push_back({p1, colorVec});
        lineVertices.push_back({p2, colorVec});

        // NOTE: Edge 2 -> 0
        lineVertices.push_back({p2, colorVec});
        lineVertices.push_back({p0, colorVec});
    }

    vertexCount_ = newVertexCount;

    if (vertexBuffer_ == nullptr || vertexBuffer_->getVertexCount() < newVertexCount) {
        // NOTE: Allocate a new (or larger) dynamic vertex buffer.
        auto [vb, err] = graphicsDevice->createVertexBuffer(
            lineVertices.data(),
            newVertexCount,
            sizeof(VertexPositionColor),
            gpu::BufferUsage::Dynamic);
        if (err != nullptr) {
            // NOTE: Silently drop the draw if allocation fails.
            vertexCount_ = 0;
            return;
        }
        vertexBuffer_ = std::move(vb);
    }
    else {
        vertexBuffer_->setData(lineVertices.data(), newVertexCount);
    }
}

void WireframeEffect::draw(
    const std::shared_ptr<gpu::CommandList>& commandList,
    const std::shared_ptr<gpu::ConstantBuffer>& modelConstantBuffer,
    const std::shared_ptr<gpu::ConstantBuffer>& worldConstantBuffer)
{
    if (vertexCount_ == 0 || pipelineState_ == nullptr || vertexBuffer_ == nullptr) {
        return;
    }

    commandList->setPipelineState(pipelineState_);
    commandList->setConstantBuffer(0, modelConstantBuffer);
    commandList->setConstantBuffer(1, worldConstantBuffer);
    commandList->setVertexBuffer(0, vertexBuffer_);
    commandList->draw(vertexCount_, 0);
}

} // namespace pomdog
