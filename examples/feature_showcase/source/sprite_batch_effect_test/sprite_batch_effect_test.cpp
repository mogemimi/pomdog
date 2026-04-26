#include "sprite_batch_effect_test.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

SpriteBatchEffectTest::SpriteBatchEffectTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
SpriteBatchEffectTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
{
    auto clock = gameHost_->getClock();

    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    // NOTE: Create SpritePipeline with Sprite shader mode (default)
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
        return errors::wrap(std::move(err), "failed to create SpritePipeline (Sprite)");
    }
    else {
        spritePipelineSprite_ = std::move(p);
    }

    // NOTE: Create SpritePipeline with SolidFill shader mode
    if (auto [p, err] = createSpritePipeline(
            fs_,
            graphicsDevice_,
            gpu::BlendDesc::createNonPremultiplied(),
            std::nullopt,
            gpu::SamplerDesc::createPointWrap(),
            std::nullopt,
            std::nullopt,
            SpriteBatchPixelShaderMode::SolidFill);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpritePipeline (SolidFill)");
    }
    else {
        spritePipelineSolidFill_ = std::move(p);
    }

    // NOTE: Create SpritePipeline with WaterLine shader mode
    if (auto [p, err] = createSpritePipeline(
            fs_,
            graphicsDevice_,
            gpu::BlendDesc::createNonPremultiplied(),
            std::nullopt,
            gpu::SamplerDesc::createPointWrap(),
            std::nullopt,
            std::nullopt,
            SpriteBatchPixelShaderMode::WaterLine);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpritePipeline (WaterLine)");
    }
    else {
        spritePipelineWaterLine_ = std::move(p);
    }

    if (auto [p, err] = createSpriteBatch(graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteBatch");
    }
    else {
        spriteBatch_ = std::move(p);
    }

    // NOTE: Load PNG texture.
    if (auto [texture, err] = loadTexture2D(fs_, graphicsDevice_, "/assets/textures/pomdog.png"); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }
    else {
        texture_ = std::move(texture);
    }

    timer_ = std::make_shared<Timer>(clock);
    timer_->setInterval(std::chrono::seconds(1));
    timer_->setScale(0.2);

    return nullptr;
}

void SpriteBatchEffectTest::update()
{
}

void SpriteBatchEffectTest::draw()
{
    const auto presentationParameters = graphicsDevice_->getPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = std::uint8_t(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    const auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<float>(presentationParameters.backBufferWidth),
        static_cast<float>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    const auto t = static_cast<float>(timer_->getTotalTime().count());
    const auto w = static_cast<float>(presentationParameters.backBufferWidth);
    const auto h = static_cast<float>(presentationParameters.backBufferHeight);
    const auto texW = texture_->getWidth();
    const auto texH = texture_->getHeight();

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));

    spriteBatch_->reset();
    spriteBatch_->setTransform(projectionMatrix);

    // NOTE: Draw with Sprite shader (normal textured sprites)
    for (int i = 0; i < 5; ++i) {
        spriteBatch_->draw(
            texture_,
            Vector2{-w * 0.4f + 60.0f * static_cast<float>(i), h * 0.3f},
            Rect2D{0, 0, texW, texH},
            Color::createWhite(),
            0.0f,
            Vector2{0.5f, 0.5f},
            1.5f);
    }
    spriteBatch_->flush(commandList_, spritePipelineSprite_);

    // NOTE: Draw with SolidFill shader (solid color fill, ignoring texture alpha for color)
    for (int i = 0; i < 5; ++i) {
        const auto color = Color{
            static_cast<std::uint8_t>(50 * i),
            static_cast<std::uint8_t>(255 - 50 * i),
            128, 255};
        spriteBatch_->draw(
            texture_,
            Vector2{-w * 0.4f + 60.0f * static_cast<float>(i), 0.0f},
            Rect2D{0, 0, texW, texH},
            SpriteBatchDrawParameters{
                .color = color,
                .blendFactor = static_cast<float>(i) * 0.25f,
                .originPivot = {0.5f, 0.5f},
                .scale = {1.5f, 1.5f},
            });
    }
    spriteBatch_->flush(commandList_, spritePipelineSolidFill_);

    // NOTE: Draw with WaterLine shader (animated water line effect)
    for (int i = 0; i < 5; ++i) {
        const auto waterLineY = std::sin(2.0f * t + static_cast<float>(i) * 0.5f) * 20.0f;
        spriteBatch_->draw(
            texture_,
            Vector2{-w * 0.4f + 60.0f * static_cast<float>(i), -h * 0.3f},
            Rect2D{0, 0, texW, texH},
            SpriteBatchDrawParameters{
                .color = Color::createWhite(),
                .color1 = Color{10, 200, 255, 100},
                .blendFactor = 0.0f,
                .waterLineYPosition = waterLineY - h * 0.3f,
                .originPivot = {0.5f, 0.5f},
                .scale = {1.5f, 1.5f},
            });
    }
    spriteBatch_->flush(commandList_, spritePipelineWaterLine_);
    spriteBatch_->submit(graphicsDevice_);

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
