#include "texture2d_loader_test.h"

namespace feature_showcase {

Texture2DLoaderTest::Texture2DLoaderTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost(gameHostIn)
    , fs_(fs)
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
Texture2DLoaderTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/, int /*argc*/, const char* const* /*argv*/)
{
    auto clock = gameHost->getClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->createCommandList();
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }

    spriteBatch = std::make_shared<SpriteBatch>();
    if (auto spriteBatchErr = spriteBatch->initialize(fs_, graphicsDevice); spriteBatchErr != nullptr) {
        return errors::wrap(std::move(spriteBatchErr), "failed to initialize SpriteBatch");
    }

    auto [font, fontErr] = loadTrueTypeFont(fs_, "/assets/fonts/NotoSans-Regular.ttf");
    if (fontErr != nullptr) {
        return errors::wrap(std::move(fontErr), "failed to load a font file");
    }

    spriteFont = std::make_shared<SpriteFont>();
    if (auto spriteFontErr = spriteFont->initialize(graphicsDevice, font, 24.0f, 24.0f); spriteFontErr != nullptr) {
        return errors::wrap(std::move(spriteFontErr), "failed to initialize SpriteFont");
    }
    spriteFont->prepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");

    // NOTE: Load PNG texture.
    std::tie(texturePNG, err) = loadTexture2D(fs_, graphicsDevice, "/assets/textures/pomdog.png");
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }

    // NOTE: Load DDS texture (DXT1 compression).
    std::tie(textureDXT1, err) = loadTexture2D(fs_, graphicsDevice, "/assets/textures/pomdog-dxt1.dds");
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }

    // NOTE: Load DDS texture (DXT5 compression).
    std::tie(textureDXT5, err) = loadTexture2D(fs_, graphicsDevice, "/assets/textures/pomdog-dxt5.dds");
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }

    // NOTE: Load PNM/Netpbm bitmap ascii texture (P1).
    std::tie(texturePNMP1, err) = loadTexture2D(fs_, graphicsDevice, "/assets/textures/pomdog-p1.pbm");
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }

    // NOTE: Load PNM/Netpbm graymap ascii texture (P2).
    std::tie(texturePNMP2, err) = loadTexture2D(fs_, graphicsDevice, "/assets/textures/pomdog-p2.pgm");
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }

    // NOTE: Load PNM/Netpbm pixmap ascii texture (P3).
    std::tie(texturePNMP3, err) = loadTexture2D(fs_, graphicsDevice, "/assets/textures/pomdog-p3.ppm");
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }

    // NOTE: Load PNM/Netpbm bitmap binary texture (P4).
    std::tie(texturePNMP4, err) = loadTexture2D(fs_, graphicsDevice, "/assets/textures/pomdog-p4.pbm");
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }

    // NOTE: Load PNM/Netpbm graymap binary texture (P5).
    std::tie(texturePNMP5, err) = loadTexture2D(fs_, graphicsDevice, "/assets/textures/pomdog-p5.pgm");
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }

    // NOTE: Load PNM/Netpbm pixmap binary texture (P6).
    std::tie(texturePNMP6, err) = loadTexture2D(fs_, graphicsDevice, "/assets/textures/pomdog-p6.ppm");
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }

    return nullptr;
}

void Texture2DLoaderTest::update()
{
}

void Texture2DLoaderTest::draw()
{
    auto presentationParameters = graphicsDevice->getPresentationParameters();

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

    commandList->reset();
    commandList->beginRenderPass(std::move(pass));

    const auto textures = {
        texturePNG,
        textureDXT1,
        textureDXT5,
        texturePNMP1,
        texturePNMP2,
        texturePNMP3,
        texturePNMP4,
        texturePNMP5,
        texturePNMP6,
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

    spriteBatch->begin(commandList, projectionMatrix);
    constexpr float marginY = 42.0f;
    constexpr float startY = 160.0f;
    float posY = startY;
    for (auto& t : textures) {
        spriteBatch->draw(t, Vector2{-100.0f, posY}, Color::createWhite());
        posY = posY - marginY;
    }
    posY = startY;
    for (auto& t : texts) {
        spriteFont->draw(*spriteBatch, t, Vector2{-60.0f, posY}, Color::createWhite(), 0.0f, Vector2{0.0f, 0.3f}, 0.8f);
        posY = posY - marginY;
    }
    spriteBatch->end();

    commandList->endRenderPass();
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
