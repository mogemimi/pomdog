#include "svg_decode_test.h"
#include "pomdog/experimental/image/svg_loader.h"

namespace feature_showcase {

SVGDecodeTest::SVGDecodeTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error> SVGDecodeTest::initialize()
{
    auto assets = gameHost->getAssetManager();
    auto clock = gameHost->getClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->createCommandList();
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }

    spriteBatch = std::make_shared<SpriteBatch>(graphicsDevice, *assets);

    auto [font, fontErr] = assets->load<TrueTypeFont>("Fonts/NotoSans/NotoSans-Regular.ttf");
    if (fontErr != nullptr) {
        return errors::wrap(std::move(fontErr), "failed to load a font file");
    }

    spriteFont = std::make_shared<SpriteFont>(graphicsDevice, font, 24.0f, 24.0f);
    spriteFont->prepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");

    svgFiles = {
        "SVG/ionicons/ios-search.svg",
        "SVG/ionicons/ios-square-outline.svg",
        "SVG/ionicons/ios-square.svg",
        "SVG/ionicons/md-add.svg",
        "SVG/ionicons/md-arrow-back.svg",
        "SVG/ionicons/md-arrow-down.svg",
        "SVG/ionicons/md-arrow-dropdown.svg",
        "SVG/ionicons/md-arrow-dropleft.svg",
        "SVG/ionicons/md-arrow-dropright.svg",
        "SVG/ionicons/md-arrow-dropup.svg",
        "SVG/ionicons/md-arrow-forward.svg",
        "SVG/ionicons/md-arrow-up.svg",
        "SVG/ionicons/md-checkmark.svg",
        "SVG/ionicons/md-close.svg",
    };

    for (auto& s : svgFiles) {
        // NOTE: Load SVG texture.
        auto filePath = filepaths::normalize(filepaths::join(assets->getContentDirectory(), s));
        constexpr int canvasWidth = 24;
        constexpr int canvasHeight = 24;

        auto [res, loadErr] = SVG::LoadTexture(graphicsDevice, filePath, canvasWidth, canvasHeight);
        if (loadErr != nullptr) {
            return errors::wrap(std::move(err), "failed to load texture");
        }
        textures.push_back(std::move(res));
    }

    return nullptr;
}

void SVGDecodeTest::update()
{
}

void SVGDecodeTest::draw()
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
    commandList->setRenderPass(std::move(pass));

    spriteBatch->begin(commandList, projectionMatrix);
    constexpr float marginY = 32.0f;
    constexpr float startY = 210.0f;
    float posY = startY;
    for (auto& t : textures) {
        spriteBatch->draw(t, Vector2{-100.0f, posY}, Color::createWhite());
        posY = posY - marginY;
    }
    posY = startY;
    for (auto& t : svgFiles) {
        spriteFont->draw(*spriteBatch, t, Vector2{-60.0f, posY}, Color::createWhite(), 0.0f, Vector2{0.0f, 0.3f}, 0.8f);
        posY = posY - marginY;
    }
    spriteBatch->end();

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
