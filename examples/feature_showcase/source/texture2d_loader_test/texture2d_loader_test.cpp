#include "texture2d_loader_test.h"

namespace feature_showcase {

Texture2DLoaderTest::Texture2DLoaderTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
Texture2DLoaderTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
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

    // NOTE: Load PNG texture.
    if (auto [texturePNG, err] = loadTexture2D(fs_, graphicsDevice_, "/assets/textures/pomdog.png"); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }
    else {
        texturePNG_ = std::move(texturePNG);
    }

    // NOTE: Load DDS texture (DXT1 compression).
    if (auto [textureDXT1, err] = loadTexture2D(fs_, graphicsDevice_, "/assets/textures/pomdog-dxt1.dds"); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }
    else {
        textureDXT1_ = std::move(textureDXT1);
    }

    // NOTE: Load DDS texture (DXT5 compression).
    if (auto [textureDXT5, err] = loadTexture2D(fs_, graphicsDevice_, "/assets/textures/pomdog-dxt5.dds"); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }
    else {
        textureDXT5_ = std::move(textureDXT5);
    }

    // NOTE: Load PNM/Netpbm bitmap ascii texture (P1).
    if (auto [texturePNMP1, err] = loadTexture2D(fs_, graphicsDevice_, "/assets/textures/pomdog-p1.pbm"); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }
    else {
        texturePNMP1_ = std::move(texturePNMP1);
    }

    // NOTE: Load PNM/Netpbm graymap ascii texture (P2).
    if (auto [texturePNMP2, err] = loadTexture2D(fs_, graphicsDevice_, "/assets/textures/pomdog-p2.pgm"); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }
    else {
        texturePNMP2_ = std::move(texturePNMP2);
    }

    // NOTE: Load PNM/Netpbm pixmap ascii texture (P3).
    if (auto [texturePNMP3, err] = loadTexture2D(fs_, graphicsDevice_, "/assets/textures/pomdog-p3.ppm"); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }
    else {
        texturePNMP3_ = std::move(texturePNMP3);
    }

    // NOTE: Load PNM/Netpbm bitmap binary texture (P4).
    if (auto [texturePNMP4, err] = loadTexture2D(fs_, graphicsDevice_, "/assets/textures/pomdog-p4.pbm"); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }
    else {
        texturePNMP4_ = std::move(texturePNMP4);
    }

    // NOTE: Load PNM/Netpbm graymap binary texture (P5).
    if (auto [texturePNMP5, err] = loadTexture2D(fs_, graphicsDevice_, "/assets/textures/pomdog-p5.pgm"); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }
    else {
        texturePNMP5_ = std::move(texturePNMP5);
    }

    // NOTE: Load PNM/Netpbm pixmap binary texture (P6).
    if (auto [texturePNMP6, err] = loadTexture2D(fs_, graphicsDevice_, "/assets/textures/pomdog-p6.ppm"); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }
    else {
        texturePNMP6_ = std::move(texturePNMP6);
    }

    return nullptr;
}

void Texture2DLoaderTest::update()
{
}

void Texture2DLoaderTest::draw()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<f32>(presentationParameters.backBufferWidth),
        static_cast<f32>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

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

    const auto textures = {
        texturePNG_,
        textureDXT1_,
        textureDXT5_,
        texturePNMP1_,
        texturePNMP2_,
        texturePNMP3_,
        texturePNMP4_,
        texturePNMP5_,
        texturePNMP6_,
    };
    const auto texts = {
        "PNG",
        "DDS (DXT1)",
        "DDS (DXT5)",
        "PNM/Netpbm Bitmap/ASCII (P1)",
        "PNM/Netpbm Graymap/ASCII (P2)",
        "PNM/Netpbm Pixmap/ASCII (P3)",
        "PNM/Netpbm Bitmap/Binary (P4)",
        "PNM/Netpbm Graymap/Binary (P5)",
        "PNM/Netpbm Pixmap/Binary (P6)",
    };

    spriteBatch_->reset();
    spriteBatch_->setTransform(projectionMatrix);
    constexpr float marginY = 42.0f;
    constexpr float startY = 160.0f;
    float posY = startY;
    for (auto& t : textures) {
        spriteBatch_->draw(t, Vector2{-100.0f, posY}, Rect2D{0, 0, t->getWidth(), t->getHeight()}, Color::createWhite());
        posY = posY - marginY;
    }
    spriteBatch_->flush(commandList_, spritePipeline_);

    posY = startY;
    for (auto& t : texts) {
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
