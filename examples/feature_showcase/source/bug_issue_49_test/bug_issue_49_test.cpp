#include "bug_issue_49_test.h"
#include <cmath>
#include <random>

namespace feature_showcase {

BugIssue49Test::BugIssue49Test(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error> BugIssue49Test::initialize()
{
    auto assets = gameHost->getAssetManager();
    auto clock = gameHost->getClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->createCommandList();
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }

    // NOTE: Load texture from PNG image file.
    std::tie(texture, err) = assets->load<gpu::Texture2D>("Textures/pomdog.png");
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
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
            VertexCombined{Vector3{-1.0f,  1.0f, 0.0f}, Vector2{0.0f, 0.0f}},
            VertexCombined{Vector3{ 1.0f,  1.0f, 0.0f}, Vector2{1.0f, 0.0f}},
            VertexCombined{Vector3{ 1.0f, -1.0f, 0.0f}, Vector2{1.0f, 1.0f}},
        }};

        std::tie(vertexBuffer, err) = graphicsDevice->createVertexBuffer(
            verticesCombo.data(),
            verticesCombo.size(),
            sizeof(VertexCombined),
            gpu::BufferUsage::Immutable);

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create vertex buffer");
        }
    }
    {
        // NOTE: Create index buffer
        std::array<std::uint16_t, 6> indices = {{0, 1, 2, 2, 3, 0}};

        std::tie(indexBuffer, err) = graphicsDevice->createIndexBuffer(
            gpu::IndexFormat::UInt16,
            indices.data(),
            indices.size(),
            gpu::BufferUsage::Immutable);

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create index buffer");
        }
    }
    {
        // NOTE: Create instance buffer
        std::tie(instanceBuffer, err) = graphicsDevice->createVertexBuffer(
            maxSpriteCount,
            sizeof(SpriteInfo),
            gpu::BufferUsage::Dynamic);

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create instance buffer");
        }
    }
    {
        // NOTE: Create constant buffer
        std::tie(constantBuffer, err) = graphicsDevice->createConstantBuffer(
            sizeof(Matrix4x4),
            gpu::BufferUsage::Dynamic);

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create constant buffer");
        }
    }
    {
        // NOTE: Create sampler state
        std::tie(sampler, err) = graphicsDevice->createSamplerState(
            gpu::SamplerDescriptor::createLinearClamp());

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create sampler state");
        }
    }
    {
        // For details, see 'struct VertexCombined' members
        auto inputLayout = gpu::InputLayoutHelper{}
            .addFloat3() // NOTE: VertexCombined::Position
            .addFloat2() // NOTE: VertexCombined::TextureCoord
            .addInputSlot(gpu::InputClassification::InputPerInstance, 1)
            .addFloat4() // NOTE: SpriteInfo::Translation
            .addFloat4() // NOTE: SpriteInfo::Color
            .createInputLayout();

        // NOTE: Create vertex shader
        auto [vertexShader, vertexShaderErr] = assets->createBuilder<gpu::Shader>(gpu::ShaderPipelineStage::VertexShader)
            .setGLSLFromFile("Shaders/bug_issue_49_vs.glsl")
            .setHLSLFromFile("Shaders/bug_issue_49.hlsl", "BugIssue49_VS")
            .setMetalFromFile("Shaders/bug_issue_49.metal", "BugIssue49_VS")
            .build();

        if (vertexShaderErr != nullptr) {
            return errors::wrap(std::move(vertexShaderErr), "failed to create vertex shader");
        }

        // NOTE: Create pixel shader
        auto [pixelShader, pixelShaderErr] = assets->createBuilder<gpu::Shader>(gpu::ShaderPipelineStage::PixelShader)
            .setGLSLFromFile("Shaders/bug_issue_49_ps.glsl")
            .setHLSLFromFile("Shaders/bug_issue_49.hlsl", "BugIssue49_PS")
            .setMetalFromFile("Shaders/bug_issue_49.metal", "BugIssue49_PS")
            .build();

        if (pixelShaderErr != nullptr) {
            return errors::wrap(std::move(pixelShaderErr), "failed to create pixel shader");
        }

        auto presentationParameters = graphicsDevice->getPresentationParameters();

        // NOTE: Create pipeline state
        std::tie(pipelineState, err) = assets->createBuilder<gpu::PipelineState>()
            .setRenderTargetViewFormat(presentationParameters.backBufferFormat)
            .setDepthStencilViewFormat(presentationParameters.depthStencilFormat)
            .setInputLayout(inputLayout)
            .setPrimitiveTopology(gpu::PrimitiveTopology::TriangleList)
            .setVertexShader(std::move(vertexShader))
            .setPixelShader(std::move(pixelShader))
            .setConstantBufferBindSlot("MyShaderConstants", 0)
            .setSamplerBindSlot("DiffuseTexture", 2) // NOTE: not zero to reproduce the issue
            .build();

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create pipeline state");
        }
    }
    {
        SpriteInfo sprite;
        sprite.Translation = Vector4{0.0f, 0.0f, 32.0f, 32.0f};
        sprite.Color = Color::createWhite().toVector4();

        // NOTE: Add new sprite
        sprites.push_back(std::move(sprite));
    }

    auto mouse = gameHost->getMouse();
    connect(mouse->ButtonDown, [this](MouseButtons mouseButton) {
        if (mouseButton != MouseButtons::Left) {
            return;
        }

        if (sprites.size() >= instanceBuffer->getVertexCount()) {
            return;
        }

        const auto window = gameHost->getWindow();
        const auto mouse = gameHost->getMouse();
        const auto mouseState = mouse->getState();
        const auto clientBounds = window->getClientBounds();

        auto pos = mouseState.position;
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

        // NOTE: Add new sprite
        sprites.push_back(std::move(sprite));
    });

    return nullptr;
}

void BugIssue49Test::update()
{
}

void BugIssue49Test::draw()
{
    auto presentationParameters = graphicsDevice->getPresentationParameters();

    auto viewMatrix = Matrix4x4::createIdentity();
    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<float>(presentationParameters.backBufferWidth),
        static_cast<float>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);
    auto viewProjection = viewMatrix * projectionMatrix;

    // NOTE: Update constant buffer
    constantBuffer->setData(0, gpu::makeByteSpan(viewProjection));

    // NOTE: Update instance buffer
    instanceBuffer->setData(sprites.data(), sprites.size());

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = std::uint8_t(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    commandList->reset();
    commandList->setRenderPass(std::move(pass));
    commandList->setPipelineState(pipelineState);
    commandList->setConstantBuffer(0, constantBuffer);
    commandList->setSamplerState(2, sampler); // NOTE: not zero to reproduce the issue
    commandList->setTexture(2, texture);      // NOTE: not zero to reproduce the issue
    commandList->setVertexBuffer(0, vertexBuffer);
    commandList->setVertexBuffer(1, instanceBuffer);
    commandList->setIndexBuffer(indexBuffer);
    commandList->drawIndexedInstanced(indexBuffer->getIndexCount(), sprites.size(), 0, 0);
    commandList->close();

    constexpr bool isStandalone = false;
    if constexpr (isStandalone) {
        commandQueue->reset();
        commandQueue->pushBackCommandList(commandList);
        commandQueue->executeCommandLists();
        commandQueue->present();
    }
    else {
        commandQueue->pushBackCommandList(commandList);
    }
}

} // namespace feature_showcase
