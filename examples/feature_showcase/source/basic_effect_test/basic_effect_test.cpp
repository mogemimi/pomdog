#include "basic_effect_test.h"
#include "pomdog/experimental/graphics/basic_effect.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

BasicEffectTest::BasicEffectTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
BasicEffectTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/, int /*argc*/, const char* const* /*argv*/)
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

        if (auto [vertexBuffer1, err] = graphicsDevice_->createVertexBuffer(
                verticesCombo.data(),
                static_cast<u32>(verticesCombo.size()),
                sizeof(VertexCombined),
                gpu::BufferUsage::Immutable);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create vertex buffer");
        }
        else {
            vertexBuffer1_ = std::move(vertexBuffer1);
        }
    }
    {
        using VertexCombined = BasicEffect::VertexPositionColor;

        // NOTE: Create vertex buffer
        std::array<VertexCombined, 20> verticesCombo = {{
            // top
            {Vector3{0.0f, 1.0f, 0.0f}, Color::createWhite().toVector4()},
            {Vector3{1.0f, 1.0f, 0.0f}, Color::createWhite().toVector4()},
            {Vector3{1.0f, 1.0f, 1.0f}, Color::createWhite().toVector4()},
            {Vector3{0.0f, 1.0f, 1.0f}, Color::createWhite().toVector4()},

            // left
            {Vector3{0.0f, 0.0f, 1.0f}, Color::createYellow().toVector4()},
            {Vector3{0.0f, 0.0f, 0.0f}, Color::createYellow().toVector4()},
            {Vector3{0.0f, 1.0f, 0.0f}, Color::createYellow().toVector4()},
            {Vector3{0.0f, 1.0f, 1.0f}, Color::createYellow().toVector4()},

            // right
            {Vector3{1.0f, 0.0f, 1.0f}, Color::createRed().toVector4()},
            {Vector3{1.0f, 0.0f, 0.0f}, Color::createRed().toVector4()},
            {Vector3{1.0f, 1.0f, 0.0f}, Color::createRed().toVector4()},
            {Vector3{1.0f, 1.0f, 1.0f}, Color::createRed().toVector4()},

            // front
            {Vector3{0.0f, 0.0f, 0.0f}, Color::createBlue().toVector4()},
            {Vector3{1.0f, 0.0f, 0.0f}, Color::createBlue().toVector4()},
            {Vector3{1.0f, 1.0f, 0.0f}, Color::createBlue().toVector4()},
            {Vector3{0.0f, 1.0f, 0.0f}, Color::createBlue().toVector4()},

            // back
            {Vector3{0.0f, 0.0f, 1.0f}, Color::createRed().toVector4()},
            {Vector3{1.0f, 0.0f, 1.0f}, Color::createYellow().toVector4()},
            {Vector3{1.0f, 1.0f, 1.0f}, Color::createBlue().toVector4()},
            {Vector3{0.0f, 1.0f, 1.0f}, Color::createWhite().toVector4()},
        }};

        if (auto [vertexBuffer2, err] = graphicsDevice_->createVertexBuffer(
                verticesCombo.data(),
                static_cast<u32>(verticesCombo.size()),
                sizeof(VertexCombined),
                gpu::BufferUsage::Immutable);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create vertex buffer");
        }
        else {
            vertexBuffer2_ = std::move(vertexBuffer2);
        }
    }
    {
        // NOTE: Create index buffer
        std::array<std::uint16_t, 30> indices = {{// NOTE: index data for a cube
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
        auto presentationParameters = graphicsDevice_->getPresentationParameters();

        BasicEffect::BasicEffectVariant variant = BasicEffect::BasicEffectVariant::PositionNormalTexture;

        auto [pipelineDesc, basicEffectErr] = BasicEffect::createBasicEffect(fs_, graphicsDevice_, variant);
        if (basicEffectErr != nullptr) {
            return errors::wrap(std::move(basicEffectErr), "failed to create basic effect");
        }
        pipelineDesc.renderTargetViewFormats = {presentationParameters.backBufferFormat};
        pipelineDesc.depthStencilViewFormat = presentationParameters.depthStencilFormat;
        pipelineDesc.primitiveTopology = gpu::PrimitiveTopology::TriangleList;
        pipelineDesc.depthStencilState = gpu::DepthStencilDesc::createDefault();
        pipelineDesc.blendState = gpu::BlendDesc::createNonPremultiplied();
        pipelineDesc.rasterizerState = gpu::RasterizerDesc::createDefault();

        // NOTE: Create pipeline state
        if (auto [pipelineState1, err] = graphicsDevice_->createPipelineState(pipelineDesc); err != nullptr) {
            return errors::wrap(std::move(err), "failed to create pipeline state");
        }
        else {
            pipelineState1_ = std::move(pipelineState1);
        }
    }
    {
        auto presentationParameters = graphicsDevice_->getPresentationParameters();

        BasicEffect::BasicEffectVariant variant = BasicEffect::BasicEffectVariant::PositionColor;

        auto [pipelineDesc, basicEffectErr] = BasicEffect::createBasicEffect(fs_, graphicsDevice_, variant);
        if (basicEffectErr != nullptr) {
            return errors::wrap(std::move(basicEffectErr), "failed to create basic effect");
        }
        pipelineDesc.renderTargetViewFormats = {presentationParameters.backBufferFormat};
        pipelineDesc.depthStencilViewFormat = presentationParameters.depthStencilFormat;
        pipelineDesc.primitiveTopology = gpu::PrimitiveTopology::TriangleList;
        pipelineDesc.depthStencilState = gpu::DepthStencilDesc::createDefault();
        pipelineDesc.blendState = gpu::BlendDesc::createNonPremultiplied();
        pipelineDesc.rasterizerState = gpu::RasterizerDesc::createDefault();

        // NOTE: Create pipeline state
        if (auto [pipelineState2, err] = graphicsDevice_->createPipelineState(pipelineDesc); err != nullptr) {
            return errors::wrap(std::move(err), "failed to create pipeline state");
        }
        else {
            pipelineState2_ = std::move(pipelineState2);
        }
    }

    return nullptr;
}

void BasicEffectTest::update()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    constexpr float rotateSpeed = 0.5f;

    auto projectionMatrix = Matrix4x4::createPerspectiveFieldOfViewLH(
        math::toRadian(45.0f),
        static_cast<float>(presentationParameters.backBufferWidth) / presentationParameters.backBufferHeight,
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

    auto time = static_cast<float>(gameHost_->getClock()->getTotalGameTime().count());
    auto rotateY = math::TwoPi<float> * rotateSpeed * time;

    auto mouse = gameHost_->getMouse()->getState();
    if (mouse.leftButton == ButtonState::Down) {
        rotateY = -math::TwoPi<float> * (static_cast<float>(mouse.position.x) / static_cast<float>(presentationParameters.backBufferWidth));
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

void BasicEffectTest::draw()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = std::uint8_t(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    auto mouse = gameHost_->getMouse()->getState();

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));
    commandList_->setConstantBuffer(0, modelConstantBuffer_);
    commandList_->setConstantBuffer(1, worldConstantBuffer_);
    commandList_->setSamplerState(0, sampler_);
    commandList_->setTexture(0, texture_);
    if (mouse.rightButton == ButtonState::Down) {
        commandList_->setVertexBuffer(0, vertexBuffer2_);
        commandList_->setPipelineState(pipelineState2_);
    }
    else {
        commandList_->setVertexBuffer(0, vertexBuffer1_);
        commandList_->setPipelineState(pipelineState1_);
    }
    commandList_->setIndexBuffer(indexBuffer_);
    commandList_->drawIndexed(indexBuffer_->getIndexCount(), 0);
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
