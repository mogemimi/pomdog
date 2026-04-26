#include "svg_decode_test.h"
#include "pomdog/experimental/image/svg_loader.h"

namespace feature_showcase {

SVGDecodeTest::SVGDecodeTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
SVGDecodeTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
{
    auto clock = gameHost_->getClock();

    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    if (auto [p, err] = createSpritePipeline(
            fs_,
            graphicsDevice_,
            std::nullopt,
            std::nullopt,
            std::nullopt,
            std::nullopt,
            std::nullopt,
            SpriteBatchPixelShaderMode::Sprite);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpritePipeline");
    }
    else {
        spritePipeline_ = std::move(p);
    }
    if (auto [p, err] = createSpritePipeline(
            fs_,
            graphicsDevice_,
            gpu::BlendDesc::createNonPremultiplied(),
            std::nullopt,
            gpu::SamplerDesc::createLinearClamp(),
            std::nullopt,
            std::nullopt,
            SpriteBatchPixelShaderMode::DistanceField);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpritePipeline (DistanceField)");
    }
    else {
        spritePipelineFont_ = std::move(p);
    }
    if (auto [p, err] = createSpriteBatch(graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteBatch");
    }
    else {
        spriteBatch_ = std::move(p);
    }

    auto [font, fontErr] = loadTrueTypeFont(fs_, "/assets/fonts/NotoSans-Regular.ttf");
    if (fontErr != nullptr) {
        return errors::wrap(std::move(fontErr), "failed to load a font file");
    }

    constexpr bool useSDF = true;

    if (auto [p, err] = createSpriteFont(graphicsDevice_, font, 24.0f, 24.0f, useSDF); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteFont");
    }
    else {
        spriteFont_ = std::move(p);
    }

    svgFiles_ = {
        "/assets/svg/ios-search.svg",
        "/assets/svg/ios-square-outline.svg",
        "/assets/svg/ios-square.svg",
        "/assets/svg/md-add.svg",
        "/assets/svg/md-arrow-back.svg",
        "/assets/svg/md-arrow-down.svg",
        "/assets/svg/md-arrow-dropdown.svg",
        "/assets/svg/md-arrow-dropleft.svg",
        "/assets/svg/md-arrow-dropright.svg",
        "/assets/svg/md-arrow-dropup.svg",
        "/assets/svg/md-arrow-forward.svg",
        "/assets/svg/md-arrow-up.svg",
        "/assets/svg/md-checkmark.svg",
        "/assets/svg/md-close.svg",
    };

    for (const auto& file : svgFiles_) {
        // NOTE: Load SVG texture.
        constexpr int canvasWidth = 24;
        constexpr int canvasHeight = 24;

        auto [res, loadErr] = loadTextureFromSVGFile(fs_, graphicsDevice_, file, canvasWidth, canvasHeight);
        if (loadErr != nullptr) {
            return errors::wrap(std::move(loadErr), "failed to load texture");
        }
        textures_.push_back(std::move(res));
    }

    return nullptr;
}

void SVGDecodeTest::update()
{
}

void SVGDecodeTest::draw()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<float>(presentationParameters.backBufferWidth),
        static_cast<float>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = std::uint8_t(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));

    spriteBatch_->reset();
    spriteBatch_->setTransform(projectionMatrix);
    constexpr float marginY = 32.0f;
    constexpr float startY = 210.0f;
    float posY = startY;
    for (auto& t : textures_) {
        spriteBatch_->draw(t, Vector2{-100.0f, posY}, Rect2D{0, 0, t->getWidth(), t->getHeight()}, Color::createWhite());
        posY = posY - marginY;
    }
    spriteBatch_->flush(commandList_, spritePipeline_);

    posY = startY;
    for (auto& t : svgFiles_) {
        spriteFont_->draw(graphicsDevice_, *spriteBatch_, t, Vector2{-60.0f, posY}, Color::createWhite(), 0.0f, Vector2{0.0f, 0.3f}, 0.8f);
        posY = posY - marginY;
    }
    spriteBatch_->flush(commandList_, spritePipelineFont_);
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
