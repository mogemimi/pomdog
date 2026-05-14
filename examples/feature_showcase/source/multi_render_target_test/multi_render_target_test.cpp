#include "multi_render_target_test.h"
#include "pomdog/experimental/graphics/basic_effect.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

MultiRenderTargetTest::MultiRenderTargetTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
MultiRenderTargetTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
{
    auto clock = gameHost_->getClock();

    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    // NOTE: Load texture from image file
    if (auto [texture, err] = loadTexture2D(fs_, graphicsDevice_, "/assets/textures/pomdog.png"); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }
    else {
        texture_ = std::move(texture);
    }

    if (auto [p, err] = createSpritePipeline(
            fs_,
            graphicsDevice_,
            gpu::BlendDesc::createNonPremultiplied(),
            std::nullopt,
            gpu::SamplerDesc::createPointWrap(),
            std::nullopt,
            std::nullopt,
            SpriteBatchPixelShaderMode::Sprite);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpritePipeline");
    }
    else {
        spritePipeline_ = std::move(p);
    }
    if (auto [p, err] = createSpriteBatch(graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteBatch");
    }
    else {
        spriteBatch_ = std::move(p);
    }

    {
        using VertexCombined = BasicEffect::VertexPositionNormalTexture;

        // NOTE: Create vertex buffer
        std::array<VertexCombined, 20> verticesCombo = {{
            // top
            {Vector3{0.0f, 1.0f, 0.0f}, Vector3{0.0f, 1.0f, 0.0f}, Vector2{0.0f, 1.0f}},
            {Vector3{1.0f, 1.0f, 0.0f}, Vector3{0.0f, 1.0f, 0.0f}, Vector2{1.0f, 1.0f}},
            {Vector3{1.0f, 1.0f, 1.0f}, Vector3{0.0f, 1.0f, 0.0f}, Vector2{1.0f, 0.0f}},
            {Vector3{0.0f, 1.0f, 1.0f}, Vector3{0.0f, 1.0f, 0.0f}, Vector2{0.0f, 0.0f}},

            // left
            {Vector3{0.0f, 0.0f, 1.0f}, Vector3{-1.0f, 0.0f, 0.0f}, Vector2{0.0f, 1.0f}},
            {Vector3{0.0f, 0.0f, 0.0f}, Vector3{-1.0f, 0.0f, 0.0f}, Vector2{1.0f, 1.0f}},
            {Vector3{0.0f, 1.0f, 0.0f}, Vector3{-1.0f, 0.0f, 0.0f}, Vector2{1.0f, 0.0f}},
            {Vector3{0.0f, 1.0f, 1.0f}, Vector3{-1.0f, 0.0f, 0.0f}, Vector2{0.0f, 0.0f}},

            // right
            {Vector3{1.0f, 0.0f, 1.0f}, Vector3{1.0f, 0.0f, 0.0f}, Vector2{0.0f, 0.0f}},
            {Vector3{1.0f, 0.0f, 0.0f}, Vector3{1.0f, 0.0f, 0.0f}, Vector2{1.0f, 0.0f}},
            {Vector3{1.0f, 1.0f, 0.0f}, Vector3{1.0f, 0.0f, 0.0f}, Vector2{1.0f, 1.0f}},
            {Vector3{1.0f, 1.0f, 1.0f}, Vector3{1.0f, 0.0f, 0.0f}, Vector2{0.0f, 1.0f}},

            // front
            {Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, -1.0f}, Vector2{0.0f, 0.0f}},
            {Vector3{1.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, -1.0f}, Vector2{1.0f, 0.0f}},
            {Vector3{1.0f, 1.0f, 0.0f}, Vector3{0.0f, 0.0f, -1.0f}, Vector2{1.0f, 1.0f}},
            {Vector3{0.0f, 1.0f, 0.0f}, Vector3{0.0f, 0.0f, -1.0f}, Vector2{0.0f, 1.0f}},

            // back
            {Vector3{0.0f, 0.0f, 1.0f}, Vector3{0.0f, 0.0f, 1.0f}, Vector2{0.0f, 0.0f}},
            {Vector3{1.0f, 0.0f, 1.0f}, Vector3{0.0f, 0.0f, 1.0f}, Vector2{1.0f, 0.0f}},
            {Vector3{1.0f, 1.0f, 1.0f}, Vector3{0.0f, 0.0f, 1.0f}, Vector2{1.0f, 1.0f}},
            {Vector3{0.0f, 1.0f, 1.0f}, Vector3{0.0f, 0.0f, 1.0f}, Vector2{0.0f, 1.0f}},
        }};

        if (auto [vertexBuffer, err] = graphicsDevice_->createVertexBuffer(
                verticesCombo.data(),
                static_cast<u32>(verticesCombo.size()),
                sizeof(VertexCombined),
                gpu::BufferUsage::Immutable);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create vertex buffer");
        }
        else {
            vertexBuffer_ = std::move(vertexBuffer);
        }
    }
    {
        // NOTE: Create index buffer
        std::array<u16, 30> indices = {{// NOTE: index data for a cube
            // top
            3, 1, 0,
            2, 1, 3,

            // left
            7, 5, 4,
            6, 5, 7,

            // right
            10, 8, 9,
            11, 8, 10,

            // front
            15, 13, 12,
            14, 13, 15,

            // back
            18, 16, 17,
            19, 16, 18}};

        if (auto [indexBuffer, err] = graphicsDevice_->createIndexBuffer(
                gpu::IndexFormat::UInt16,
                indices.data(),
                static_cast<u32>(indices.size()),
                gpu::BufferUsage::Immutable);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create index buffer");
        }
        else {
            indexBuffer_ = std::move(indexBuffer);
        }
    }
    {
        // NOTE: Create constant buffer
        if (auto [modelConstantBuffer, err] = graphicsDevice_->createConstantBuffer(
                sizeof(BasicEffect::ModelConstantBuffer),
                gpu::BufferUsage::Dynamic);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create constant buffer");
        }
        else {
            modelConstantBuffer_ = std::move(modelConstantBuffer);
        }

        if (auto [worldConstantBuffer, err] = graphicsDevice_->createConstantBuffer(
                sizeof(BasicEffect::WorldConstantBuffer),
                gpu::BufferUsage::Dynamic);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create constant buffer");
        }
        else {
            worldConstantBuffer_ = std::move(worldConstantBuffer);
        }
    }
    {
        // NOTE: Create sampler state
        if (auto [sampler, err] = graphicsDevice_->createSamplerState(
                gpu::SamplerDesc::createLinearClamp());
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create sampler state");
        }
        else {
            sampler_ = std::move(sampler);
        }
    }
    {
        // For details, see 'struct VertexCombined' members
        gpu::InputLayoutDesc inputLayout = {};
        gpu::InputLayoutBuilder::addVertex(inputLayout,
            0, gpu::InputClassification::PerVertex, 32,
            {
                gpu::InputElementFormat::Float32x3, // NOTE: VertexCombined::Position
                gpu::InputElementFormat::Float32x3, // NOTE: VertexCombined::Normal
                gpu::InputElementFormat::Float32x2, // NOTE: VertexCombined::TextureCoord
            });

        // NOTE: Create shaders
        std::shared_ptr<gpu::Shader> vertexShader;
        std::shared_ptr<gpu::Shader> pixelShader;

        if (auto [shader, err] = loadShaderAutomagically(
                fs_,
                graphicsDevice_,
                gpu::ShaderPipelineStage::VertexShader,
                "/assets/shaders",
                "multi_rt_vs",
                "MultiRTVS");
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
                "multi_rt_ps",
                "MultiRTPS");
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to load pixel shader");
        }
        else {
            pixelShader = std::move(shader);
        }

        auto presentationParameters = graphicsDevice_->getPresentationParameters();

        gpu::PipelineDesc pipelineDesc = {};
        pipelineDesc.renderTargetViewFormats = {
            gpu::PixelFormat::R8G8B8A8_UNorm,    // NOTE: Albedo
            gpu::PixelFormat::R10G10B10A2_UNorm, // NOTE: Normal
            gpu::PixelFormat::R32_Float,         // NOTE: Depth
            gpu::PixelFormat::R8G8B8A8_UNorm,    // NOTE: Lighting
        };
        pipelineDesc.depthStencilViewFormat = presentationParameters.depthStencilFormat;
        pipelineDesc.primitiveTopology = gpu::PrimitiveTopology::TriangleList;
        pipelineDesc.depthStencilState = gpu::DepthStencilDesc::createDefault();
        pipelineDesc.blendState = gpu::BlendDesc::createOpaque();
        pipelineDesc.rasterizerState = gpu::RasterizerDesc::createDefault();
        pipelineDesc.inputLayout = inputLayout;
        pipelineDesc.vertexShader = std::move(vertexShader);
        pipelineDesc.pixelShader = std::move(pixelShader);
        pipelineDesc.multiSampleMask = std::numeric_limits<u32>::max();

        // NOTE: Create pipeline state
        if (auto [pipelineState, err] = graphicsDevice_->createPipelineState(pipelineDesc); err != nullptr) {
            return errors::wrap(std::move(err), "failed to create pipeline state");
        }
        else {
            pipelineState_ = std::move(pipelineState);
        }
    }

    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    // NOTE: Create render target
    if (auto [renderTargetAlbedo, err] = graphicsDevice_->createRenderTarget2D(
            presentationParameters.backBufferWidth,
            presentationParameters.backBufferHeight,
            false,
            gpu::PixelFormat::R8G8B8A8_UNorm);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create render target");
    }
    else {
        renderTargetAlbedo_ = std::move(renderTargetAlbedo);
    }

    // NOTE: Create render target
    if (auto [renderTargetNormal, err] = graphicsDevice_->createRenderTarget2D(
            presentationParameters.backBufferWidth,
            presentationParameters.backBufferHeight,
            false,
            gpu::PixelFormat::R10G10B10A2_UNorm);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create render target");
    }
    else {
        renderTargetNormal_ = std::move(renderTargetNormal);
    }

    // NOTE: Create render target
    if (auto [renderTargetDepth, err] = graphicsDevice_->createRenderTarget2D(
            presentationParameters.backBufferWidth,
            presentationParameters.backBufferHeight,
            false,
            gpu::PixelFormat::R32_Float);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create render target");
    }
    else {
        renderTargetDepth_ = std::move(renderTargetDepth);
    }

    // NOTE: Create render target
    if (auto [renderTargetLighting, err] = graphicsDevice_->createRenderTarget2D(
            presentationParameters.backBufferWidth,
            presentationParameters.backBufferHeight,
            false,
            gpu::PixelFormat::R8G8B8A8_UNorm);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create render target");
    }
    else {
        renderTargetLighting_ = std::move(renderTargetLighting);
    }

    // NOTE: Create depth stencil buffer
    if (auto [depthStencilBuffer, err] = graphicsDevice_->createDepthStencilBuffer(
            presentationParameters.backBufferWidth,
            presentationParameters.backBufferHeight,
            presentationParameters.depthStencilFormat);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create depth stencil buffer");
    }
    else {
        depthStencilBuffer_ = std::move(depthStencilBuffer);
    }

    auto window = gameHost_->getWindow();

    connect_(window->clientSizeChanged, [this](int width, int height) {
        renderTargetAlbedo_ = std::get<0>(graphicsDevice_->createRenderTarget2D(
            width,
            height,
            false,
            renderTargetAlbedo_->getFormat()));

        renderTargetNormal_ = std::get<0>(graphicsDevice_->createRenderTarget2D(
            width,
            height,
            false,
            renderTargetNormal_->getFormat()));

        renderTargetDepth_ = std::get<0>(graphicsDevice_->createRenderTarget2D(
            width,
            height,
            false,
            renderTargetDepth_->getFormat()));

        renderTargetLighting_ = std::get<0>(graphicsDevice_->createRenderTarget2D(
            width,
            height,
            false,
            renderTargetLighting_->getFormat()));

        depthStencilBuffer_ = std::get<0>(graphicsDevice_->createDepthStencilBuffer(
            width,
            height,
            depthStencilBuffer_->getFormat()));
    });

    return nullptr;
}

void MultiRenderTargetTest::update()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    constexpr float rotateSpeed = 0.5f;

    auto projectionMatrix = Matrix4x4::createPerspectiveFieldOfViewLH(
        math::toRadian(45.0f),
        static_cast<f32>(presentationParameters.backBufferWidth) / presentationParameters.backBufferHeight,
        0.01f,
        1000.0f);

    auto cameraPosition = Vector3{0.0f, 6.0f, 0.0f};
    auto cameraDirection = Vector3{0.0f, -1.0f, 1.0f};
    auto viewMatrix = Matrix4x4::createLookAtLH(cameraPosition, cameraPosition + cameraDirection, Vector3::createUnitY());

    auto lightDirection = math::normalize(Vector3{-0.5f, -1.0f, 0.5f});

    // NOTE: Update constant buffer for world
    BasicEffect::WorldConstantBuffer worldConstants;
    worldConstants.viewProjection = viewMatrix * projectionMatrix;
    worldConstants.view = viewMatrix;
    worldConstants.projection = projectionMatrix;
    worldConstants.inverseView = math::invert(viewMatrix);
    worldConstants.lightDirection = Vector4{lightDirection, 0.0f};
    worldConstantBuffer_->setData(0, gpu::makeByteSpan(worldConstants));

    auto time = static_cast<f32>(gameHost_->getClock()->getTotalGameTime().count());
    auto rotateY = math::TwoPi<f32> * rotateSpeed * time;

    const auto mouse = gameHost_->getMouse();
    if (mouse->isButtonDown(MouseButtons::Left)) {
        rotateY = -math::TwoPi<f32> * (static_cast<f32>(mouse->getPosition().x) / static_cast<f32>(presentationParameters.backBufferWidth));
    }

    auto modelMatrix =
        Matrix4x4::createTranslation(Vector3{-0.5f, -0.5f, -0.5f}) *
        Matrix4x4::createScale(1.0f + std::cos(time * 5.0f) * 0.1f) *
        Matrix4x4::createRotationY(rotateY) *
        Matrix4x4::createTranslation(Vector3{0.0f, 0.0f, 6.0f});

    constexpr float metalness = 0.1f;

    // NOTE: Update constant buffer for model
    BasicEffect::ModelConstantBuffer modelConstants;
    modelConstants.model = modelMatrix;
    modelConstants.material = Vector4{metalness, 0.0f, 0.0f, 0.0f};
    modelConstants.color = Vector4{1.0f, 1.0f, 1.0f, 1.0f};
    modelConstantBuffer_->setData(0, gpu::makeByteSpan(modelConstants));
}

void MultiRenderTargetTest::draw()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    commandList_->reset();

    {
        gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
        gpu::RenderPass pass;
        pass.renderTargets[0] = {renderTargetAlbedo_, Color::createCornflowerBlue().toVector4()};
        pass.renderTargets[1] = {renderTargetNormal_, Vector4{0.0f, 0.0f, 1.0f, 1.0f}};
        pass.renderTargets[2] = {renderTargetDepth_, Vector4{0.0f, 0.0f, 0.0f, 1.0f}};
        pass.renderTargets[3] = {renderTargetLighting_, Color::createCornflowerBlue().toVector4()};
        pass.depthStencilBuffer = depthStencilBuffer_;
        pass.clearDepth = 1.0f;
        pass.clearStencil = u8(0);
        pass.viewport = viewport;
        pass.scissorRect = viewport.getBounds();

        commandList_->beginRenderPass(std::move(pass));
        commandList_->setConstantBuffer(0, modelConstantBuffer_);
        commandList_->setConstantBuffer(1, worldConstantBuffer_);
        commandList_->setSamplerState(0, sampler_);
        commandList_->setTexture(0, texture_);
        commandList_->setVertexBuffer(0, vertexBuffer_);
        commandList_->setPipelineState(pipelineState_);

        commandList_->setIndexBuffer(indexBuffer_);
        commandList_->drawIndexed(indexBuffer_->getIndexCount(), 0);
        commandList_->endRenderPass();
    }
    {
        gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
        gpu::RenderPass pass;
        pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
        pass.depthStencilBuffer = nullptr;
        pass.clearDepth = 1.0f;
        pass.clearStencil = u8(0);
        pass.viewport = viewport;
        pass.scissorRect = viewport.getBounds();

        const auto w = static_cast<f32>(presentationParameters.backBufferWidth);
        const auto h = static_cast<f32>(presentationParameters.backBufferHeight);
        const auto projectionMatrix = Matrix4x4::createOrthographicLH(w, h, 0.0f, 100.0f);

        commandList_->beginRenderPass(std::move(pass));

        spriteBatch_->reset();
        spriteBatch_->setTransform(projectionMatrix);

        auto draw = [&](std::shared_ptr<gpu::RenderTarget2D> rt, Vector2 pos) {
            auto originPivot = Vector2::createZero();
            auto scale = Vector2{0.5f, 0.5f};
            if (graphicsDevice_->isRenderTargetTextureFlipped()) {
                // NOTE: Flip horizontally for OpenGL coordinate system.
                originPivot.y = 1.0f;
                scale.y = -0.5f;
            }
            spriteBatch_->draw(
                rt,
                pos,
                Rect2D{0, 0, rt->getWidth(), rt->getHeight()},
                Color::createWhite(),
                0.0f,
                originPivot,
                scale);
        };

        draw(renderTargetAlbedo_, Vector2{-w / 2, 0.0f});
        draw(renderTargetNormal_, Vector2{0.0f, 0.0f});
        draw(renderTargetDepth_, Vector2{-w / 2, -h / 2});
        draw(renderTargetLighting_, Vector2{0.0f, -h / 2});

        spriteBatch_->flush(commandList_, spritePipeline_);
        spriteBatch_->submit(graphicsDevice_);
        commandList_->endRenderPass();
    }

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
