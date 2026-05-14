#include "billboard_batch_test.h"
#include "pomdog/experimental/graphics/basic_effect.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

BillboardBatchTest::BillboardBatchTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
BillboardBatchTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
{
    auto clock = gameHost_->getClock();

    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    if (auto [p, err] = createLinePipeline(fs_, graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create LinePipeline");
    }
    else {
        linePipeline_ = std::move(p);
    }
    if (auto [p, err] = createLineBatch(graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create LineBatch");
    }
    else {
        lineBatch_ = std::move(p);
    }

    // NOTE: Create billboard batch effect
    if (auto [p, err] = createBillboardBatchEffect(
            fs_,
            graphicsDevice_,
            gpu::BlendDesc::createNonPremultiplied(),
            std::nullopt,
            std::nullopt,
            std::nullopt,
            std::nullopt);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create BillboardBatchEffect");
    }
    else {
        billboardEffect_ = std::move(p);
    }

    // NOTE: Create billboard batch buffer
    if (auto [p, err] = createBillboardBatchBuffer(graphicsDevice_, 256); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create BillboardBatchBuffer");
    }
    else {
        billboardBuffer_ = std::move(p);
    }

    // NOTE: Create sampler state
    if (auto [sampler, err] = graphicsDevice_->createSamplerState(
            gpu::SamplerDesc::createLinearClamp());
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create sampler state");
    }
    else {
        sampler_ = std::move(sampler);
    }

    // NOTE: Create constant buffer
    if (auto [constantBuffer, err] = graphicsDevice_->createConstantBuffer(
            sizeof(BasicEffect::WorldConstantBuffer),
            gpu::BufferUsage::Dynamic);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create constant buffer");
    }
    else {
        constantBuffer_ = std::move(constantBuffer);
    }

    // NOTE: Load texture from PNG image file.
    if (auto [texture, err] = loadTexture2D(fs_, graphicsDevice_, "/assets/textures/pomdog.png"); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }
    else {
        texture_ = std::move(texture);
    }

    timer_ = std::make_shared<Timer>(clock);
    timer_->setInterval(std::chrono::seconds(1));
    timer_->setScale(0.1);

    return nullptr;
}

void BillboardBatchTest::update()
{
}

void BillboardBatchTest::draw()
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

    const auto projectionMatrix = Matrix4x4::createPerspectiveFieldOfViewLH(
        math::toRadian(45.0f),
        static_cast<f32>(presentationParameters.backBufferWidth) / static_cast<f32>(presentationParameters.backBufferHeight),
        0.01f,
        500.0f);

    const auto totalTime = static_cast<f32>(timer_->getTotalTime().count());
    const auto lookAtPosition = Vector3{0.0f, 0.0f, 5.0f};
    const auto rotation = Matrix4x4::createRotationY(math::TwoPi<f32> * totalTime);
    const auto cameraPosition = lookAtPosition + math::transform(Vector3{0.0f, 6.0f, -8.0f}, rotation);
    const auto viewMatrix = Matrix4x4::createLookAtLH(cameraPosition, lookAtPosition, Vector3::createUnitY());
    const auto viewProjection = viewMatrix * projectionMatrix;

    const auto lightDirection = math::normalize(Vector3{-0.5f, -1.0f, 0.5f});

    // NOTE: Update constant buffer for world
    BasicEffect::WorldConstantBuffer constants;
    constants.viewProjection = viewProjection;
    constants.view = viewMatrix;
    constants.projection = projectionMatrix;
    constants.inverseView = math::invert(viewMatrix);
    constants.lightDirection = Vector4{lightDirection, 0.0f};
    constantBuffer_->setData(0, gpu::makeByteSpan(constants));

    // Drawing line
    lineBatch_->reset();
    lineBatch_->setTransform(viewProjection);
    {
        // NOTE: Draw grid
        constexpr int lineCount = 40;
        constexpr f32 gridSize = 1.0f;
        constexpr f32 lineLength = gridSize * (lineCount - 1);
        for (int i = 0; i < lineCount; i++) {
            constexpr f32 startOffsetX = -10.0f;
            constexpr f32 startOffsetZ = -10.0f;
            f32 x = static_cast<f32>(i) * gridSize + startOffsetX;
            f32 z = static_cast<f32>(i) * gridSize + startOffsetZ;

            auto color = Color::createWhite();
            if (i % 5 != 0) {
                color = Color{255, 255, 255, 100};
            }

            lineBatch_->drawLine(Vector3{x, 0.0f, startOffsetZ}, Vector3{x, 0.0f, lineLength + startOffsetZ}, color);
            lineBatch_->drawLine(Vector3{startOffsetX, 0.0f, z}, Vector3{lineLength + startOffsetX, 0.0f, z}, color);
        }
    }
    lineBatch_->flush(commandList_, linePipeline_);
    lineBatch_->submit(graphicsDevice_);

    billboardBuffer_->reset();

    billboardBuffer_->addBillboard(
        Vector3{0.0f, 0.0f, 2.0f},
        Vector2{0.0f, 0.0f},
        Vector2{1.0f, 1.0f},
        Color::createWhite(), 0.0f,
        Vector2{0.5f, 0.5f},
        Vector2{1.0f, 1.0f});

    billboardBuffer_->addBillboard(
        Vector3{0.0f, 0.0f, 5.0f},
        Vector2{0.0f, 0.0f},
        Vector2{1.0f, 1.0f},
        Color::createRed(),
        math::toRadian(45.0f),
        Vector2{0.5f, 0.5f},
        Vector2{1.0f, 1.0f});

    billboardBuffer_->addBillboard(
        Vector3{0.0f, 0.0f, 7.0f},
        Vector2{0.0f, 0.0f},
        Vector2{0.5f, 1.0f},
        Color::createLime(),
        math::toRadian(0.0f),
        Vector2{0.5f, 0.0f},
        Vector2{1.0f, 2.0f});

    billboardBuffer_->addBillboard(
        Vector3{0.0f, 0.0f, 10.0f},
        Vector2{0.0f, 0.0f},
        Vector2{1.0f, 0.5f},
        Color::createBlue(),
        math::toRadian(0.0f),
        Vector2{0.0f, 0.0f},
        Vector2{2.0f, 1.0f});

    billboardBuffer_->fetchBuffer();
    billboardEffect_->draw(commandList_, texture_, sampler_, constantBuffer_, 0, *billboardBuffer_);

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
