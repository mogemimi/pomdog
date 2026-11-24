// Copyright mogemimi. Distributed under the MIT license.

#include "triangle_test.h"
#include "pomdog/application/game_host.h"
#include "pomdog/content/shader_loader.h"
#include "pomdog/gpu/blend_desc.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/command_queue.h"
#include "pomdog/gpu/depth_stencil_desc.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/input_classification.h"
#include "pomdog/gpu/input_element_format.h"
#include "pomdog/gpu/input_layout_builder.h"
#include "pomdog/gpu/input_layout_desc.h"
#include "pomdog/gpu/pipeline_desc.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/primitive_topology.h"
#include "pomdog/gpu/rasterizer_desc.h"
#include "pomdog/gpu/render_pass.h"
#include "pomdog/gpu/shader_pipeline_stage.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/gpu/viewport.h"
#include "pomdog/math/color.h"
#include "pomdog/math/vector3.h"
#include "pomdog/math/vector4.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {
namespace {

// NOTE: The layout of this struct must match `VSInput` in triangle.slang.
struct Vertex final {
    Vector3 position;
    Vector4 color;
};

} // namespace

TriangleTest::TriangleTest(const std::shared_ptr<vfs::FileSystemContext>& fs)
    : fs_(fs)
{
}

std::unique_ptr<Error>
TriangleTest::initialize(const std::shared_ptr<GameHost>& gameHost)
{
    graphicsDevice_ = gameHost->getGraphicsDevice();
    commandQueue_ = gameHost->getCommandQueue();

    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    {
        // NOTE: The positions are in clip space, where (-1, -1) is the
        // bottom-left corner and (1, 1) is the top-right corner of the
        // viewport, so the triangle stretches with the window. The vertices
        // are in clockwise order, because the default rasterizer state culls
        // counterclockwise faces.
        const std::array<Vertex, 3> vertices = {{
            Vertex{Vector3{0.0f, 0.5f, 0.0f}, Vector4{1.0f, 0.0f, 0.0f, 1.0f}},
            Vertex{Vector3{0.5f, -0.5f, 0.0f}, Vector4{0.0f, 1.0f, 0.0f, 1.0f}},
            Vertex{Vector3{-0.5f, -0.5f, 0.0f}, Vector4{0.0f, 0.0f, 1.0f, 1.0f}},
        }};

        // NOTE: Create vertex buffer
        if (auto [vertexBuffer, err] = graphicsDevice_->createVertexBuffer(
                vertices.data(),
                static_cast<u32>(vertices.size()),
                static_cast<u32>(sizeof(Vertex)),
                gpu::BufferUsage::Immutable);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create vertex buffer");
        }
        else {
            vertexBuffer_ = std::move(vertexBuffer);
        }
    }
    {
        gpu::InputLayoutDesc inputLayout = {};
        gpu::InputLayoutBuilder::addVertex(inputLayout,
            0, gpu::InputClassification::PerVertex, static_cast<u32>(sizeof(Vertex)),
            {
                gpu::InputElementFormat::Float32x3, // NOTE: Vertex::position
                gpu::InputElementFormat::Float32x4, // NOTE: Vertex::color
            });

        // NOTE: Load shaders
        std::shared_ptr<gpu::Shader> vertexShader;
        std::shared_ptr<gpu::Shader> pixelShader;

        if (auto [shader, err] = loadShaderAutomagically(
                fs_,
                graphicsDevice_,
                gpu::ShaderPipelineStage::VertexShader,
                "/assets/shaders",
                "triangle_vs");
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to load vertex shader");
        }
        else {
            vertexShader = std::move(shader);
        }

        if (auto [shader, err] = loadShaderAutomagically(
                fs_,
                graphicsDevice_,
                gpu::ShaderPipelineStage::PixelShader,
                "/assets/shaders",
                "triangle_ps");
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to load pixel shader");
        }
        else {
            pixelShader = std::move(shader);
        }

        const auto presentationParameters = graphicsDevice_->getPresentationParameters();

        gpu::PipelineDesc pipelineDesc = {};
        pipelineDesc.renderTargetViewFormats = {presentationParameters.backBufferFormat};
        pipelineDesc.depthStencilViewFormat = presentationParameters.depthStencilFormat;
        pipelineDesc.inputLayout = inputLayout;
        pipelineDesc.primitiveTopology = gpu::PrimitiveTopology::TriangleList;
        pipelineDesc.vertexShader = std::move(vertexShader);
        pipelineDesc.pixelShader = std::move(pixelShader);
        pipelineDesc.blendState = gpu::BlendDesc::createOpaque();
        pipelineDesc.rasterizerState = gpu::RasterizerDesc::createDefault();
        pipelineDesc.depthStencilState = gpu::DepthStencilDesc::createNone();

        // NOTE: Create pipeline state
        if (auto [pipelineState, err] = graphicsDevice_->createPipelineState(pipelineDesc); err != nullptr) {
            return errors::wrap(std::move(err), "failed to create pipeline state");
        }
        else {
            pipelineState_ = std::move(pipelineState);
        }
    }

    return nullptr;
}

void TriangleTest::update()
{
}

void TriangleTest::draw()
{
    const auto presentationParameters = graphicsDevice_->getPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = u8(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));
    commandList_->setPipelineState(pipelineState_);
    commandList_->setVertexBuffer(0, vertexBuffer_);
    commandList_->draw(vertexBuffer_->getVertexCount(), 0);
    commandList_->endRenderPass();
    commandList_->close();

    constexpr bool isStandalone = false;
    if constexpr (isStandalone) {
        commandQueue_->reset();
        commandQueue_->pushBackCommandList(commandList_);
        commandQueue_->executeCommandLists();
        commandQueue_->present();
    }
    else {
        commandQueue_->pushBackCommandList(commandList_);
    }
}

} // namespace feature_showcase
