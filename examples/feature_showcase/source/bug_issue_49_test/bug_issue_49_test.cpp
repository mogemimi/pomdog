#include "bug_issue_49_test.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

BugIssue49Test::BugIssue49Test(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
BugIssue49Test::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
{
    auto clock = gameHost_->getClock();

    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    // NOTE: Load texture from PNG image file.
    if (auto [texture, err] = loadTexture2D(fs_, graphicsDevice_, "/assets/textures/pomdog.png"); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }
    else {
        texture_ = std::move(texture);
    }

    constexpr auto maxSpriteCount = 256;

    {
        // NOTE: Create vertex buffer
        struct VertexCombined final {
            Vector3 Position;
            Vector2 TextureCoord;
        };

        std::array<VertexCombined, 4> verticesCombo = {{
            VertexCombined{Vector3{-1.0f, -1.0f, 0.0f}, Vector2{0.0f, 1.0f}},
            VertexCombined{Vector3{-1.0f, 1.0f, 0.0f}, Vector2{0.0f, 0.0f}},
            VertexCombined{Vector3{1.0f, 1.0f, 0.0f}, Vector2{1.0f, 0.0f}},
            VertexCombined{Vector3{1.0f, -1.0f, 0.0f}, Vector2{1.0f, 1.0f}},
        }};

        if (auto [vertexBuffer, err] = graphicsDevice_->createVertexBuffer(
                verticesCombo.data(),
                static_cast<u32>(verticesCombo.size()),
                static_cast<u32>(sizeof(VertexCombined)),
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
        std::array<std::uint16_t, 6> indices = {{0, 1, 2, 2, 3, 0}};

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
        // NOTE: Create instance buffer
        if (auto [instanceBuffer, err] = graphicsDevice_->createVertexBuffer(
                maxSpriteCount,
                sizeof(SpriteInfo),
                gpu::BufferUsage::Dynamic);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create instance buffer");
        }
        else {
            instanceBuffer_ = std::move(instanceBuffer);
        }
    }
    {
        // NOTE: Create constant buffer
        if (auto [constantBuffer, err] = graphicsDevice_->createConstantBuffer(
                sizeof(Matrix4x4),
                gpu::BufferUsage::Dynamic);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create constant buffer");
        }
        else {
            constantBuffer_ = std::move(constantBuffer);
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
            0, gpu::InputClassification::PerVertex, 20,
            {
                gpu::InputElementFormat::Float32x3, // NOTE: VertexCombined::Position
                gpu::InputElementFormat::Float32x2, // NOTE: VertexCombined::TextureCoord
            });
        gpu::InputLayoutBuilder::addVertex(inputLayout,
            1, gpu::InputClassification::PerInstance, 32,
            {
                gpu::InputElementFormat::Float32x4, // NOTE: SpriteInfo::Translation
                gpu::InputElementFormat::Float32x4, // NOTE: SpriteInfo::Color
            });

        // NOTE: Create shaders
        std::shared_ptr<gpu::Shader> vertexShader;
        std::shared_ptr<gpu::Shader> pixelShader;

        if (auto [shader, shaderErr] = loadShaderAutomagically(
                fs_,
                graphicsDevice_,
                gpu::ShaderPipelineStage::VertexShader,
                "/assets/shaders",
                "bug_issue_49_vs",
                "BugIssue49_VS");
            shaderErr != nullptr) {
            return errors::wrap(std::move(shaderErr), "failed to load vertex shader");
        }
        else {
            vertexShader = std::move(shader);
        }

        if (auto [shader, shaderErr] = loadShaderAutomagically(
                fs_,
                graphicsDevice_,
                gpu::ShaderPipelineStage::PixelShader,
                "/assets/shaders",
                "bug_issue_49_ps",
                "BugIssue49_PS");
            shaderErr != nullptr) {
            return errors::wrap(std::move(shaderErr), "failed to load pixel shader");
        }
        else {
            pixelShader = std::move(shader);
        }

        auto presentationParameters = graphicsDevice_->getPresentationParameters();

        gpu::PipelineDesc pipelineDesc = {};
        pipelineDesc.renderTargetViewFormats = {presentationParameters.backBufferFormat};
        pipelineDesc.depthStencilViewFormat = presentationParameters.depthStencilFormat;
        pipelineDesc.inputLayout = inputLayout;
        pipelineDesc.primitiveTopology = gpu::PrimitiveTopology::TriangleList;
        pipelineDesc.vertexShader = std::move(vertexShader);
        pipelineDesc.pixelShader = std::move(pixelShader);
        pipelineDesc.blendState = gpu::BlendDesc::createDefault();
        pipelineDesc.rasterizerState = gpu::RasterizerDesc::createDefault();
        pipelineDesc.depthStencilState = gpu::DepthStencilDesc::createDefault();

        // NOTE: Create pipeline state
        if (auto [pipelineState, err] = graphicsDevice_->createPipelineState(pipelineDesc); err != nullptr) {
            return errors::wrap(std::move(err), "failed to create pipeline state");
        }
        else {
            pipelineState_ = std::move(pipelineState);
        }
    }
    {
        SpriteInfo sprite;
        sprite.Translation = Vector4{0.0f, 0.0f, 32.0f, 32.0f};
        sprite.Color = Color::createWhite().toVector4();

        // NOTE: Add new sprite
        sprites_.push_back(std::move(sprite));
    }

    return nullptr;
}

void BugIssue49Test::update()
{
    const auto mouse = gameHost_->getMouse();
    bool clicked = false;
    if (mouse->isButtonDown(MouseButtons::Left)) {
        if (!wasLeftMouseDown_) {
            clicked = true;
        }
        wasLeftMouseDown_ = true;
    }
    else {
        wasLeftMouseDown_ = false;
    }

    if (clicked) {
        if (sprites_.size() < instanceBuffer_->getVertexCount()) {
            const auto window = gameHost_->getWindow();
            const auto clientBounds = window->getClientBounds();

            auto pos = mouse->getPosition();
            pos.x = pos.x - (clientBounds.width / 2);
            pos.y = -pos.y + (clientBounds.height / 2);

            std::mt19937 random(std::random_device{}());
            std::uniform_real_distribution<float> scaleDist(16.0f, 32.0f);
            std::uniform_real_distribution<float> colorDist{0.7f, 1.0f};

            const auto scale = scaleDist(random);

            SpriteInfo sprite;
            sprite.Translation.x = static_cast<float>(pos.x);
            sprite.Translation.y = static_cast<float>(pos.y);
            sprite.Translation.z = scale;
            sprite.Translation.w = scale;
            sprite.Color.x = colorDist(random);
            sprite.Color.y = colorDist(random);
            sprite.Color.z = colorDist(random);
            sprite.Color.w = 1.0f;

            sprites_.push_back(std::move(sprite));
        }
    }
}

void BugIssue49Test::draw()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    auto viewMatrix = Matrix4x4::createIdentity();
    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<f32>(presentationParameters.backBufferWidth),
        static_cast<f32>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);
    auto viewProjection = viewMatrix * projectionMatrix;

    // NOTE: Update constant buffer
    constantBuffer_->setData(0, gpu::makeByteSpan(viewProjection));

    // NOTE: Update instance buffer
    instanceBuffer_->setData(sprites_.data(), static_cast<u32>(sprites_.size()));

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
    commandList_->setConstantBuffer(0, constantBuffer_);
    commandList_->setSamplerState(2, sampler_); // NOTE: not zero to reproduce the issue
    commandList_->setTexture(2, texture_);      // NOTE: not zero to reproduce the issue
    commandList_->setVertexBuffer(0, vertexBuffer_);
    commandList_->setVertexBuffer(1, instanceBuffer_);
    commandList_->setIndexBuffer(indexBuffer_);
    commandList_->drawIndexedInstanced(indexBuffer_->getIndexCount(), static_cast<u32>(sprites_.size()), 0, 0);
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
