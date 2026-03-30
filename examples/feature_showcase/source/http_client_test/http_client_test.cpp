#include "http_client_test.h"

namespace feature_showcase {

HTTPClientTest::HTTPClientTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost(gameHostIn)
    , fs_(fs)
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
HTTPClientTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/, int /*argc*/, const char* const* /*argv*/)
{
    auto clock = gameHost->getClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->createCommandList();
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }

    if (auto [p, spritePipelineErr] = createSpritePipeline(
            fs_,
            graphicsDevice,
            std::nullopt,
            std::nullopt,
            std::nullopt,
            std::nullopt,
            std::nullopt,
            SpriteBatchPixelShaderMode::Default);
        spritePipelineErr != nullptr) {
        return errors::wrap(std::move(spritePipelineErr), "failed to create SpritePipeline");
    }
    else {
        spritePipeline = std::move(p);
    }
    if (auto [p, spriteBatchErr] = createSpriteBatch(graphicsDevice); spriteBatchErr != nullptr) {
        return errors::wrap(std::move(spriteBatchErr), "failed to create SpriteBatch");
    }
    else {
        spriteBatch = std::move(p);
    }

    auto [font, fontErr] = loadTrueTypeFont(fs_, "/assets/fonts/NotoSans-Regular.ttf");
    if (fontErr != nullptr) {
        return errors::wrap(std::move(fontErr), "failed to load a font file");
    }

    constexpr bool useSDF = false;

    if (auto [p, spriteFontErr] = createSpriteFont(graphicsDevice, font, 24.0f, 24.0f, useSDF); spriteFontErr != nullptr) {
        return errors::wrap(std::move(spriteFontErr), "failed to create SpriteFont");
    }
    else {
        spriteFont = std::move(p);
    }
    spriteFont->prepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");

    auto http = gameHost->getHTTPClient();
    auto callback = [this](const std::shared_ptr<HTTPResponse>& resp, const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            webText = err->toString();
            return;
        }
        webText = std::string{resp->Body.data(), resp->Body.size()};
        requestURL = resp->Request->URL;

        size_t i = 0;
        for (auto& s : webText) {
            if (s == ' ') {
                ++i;
                if (i > 8) {
                    s = '\n';
                    i = 0;
                }
            }
            else if (s == '\n') {
                i = 0;
            }
        }
    };
    auto [conn, connErr] = http->Get("https://www.google.com/humans.txt", std::move(callback));
    if (connErr != nullptr) {
        webText = connErr->toString();
    }
    connect += std::move(conn);

    return nullptr;
}

void HTTPClientTest::update()
{
}

void HTTPClientTest::draw()
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

    spriteBatch->begin(commandList, spritePipeline, projectionMatrix);
    spriteFont->draw(*spriteBatch, requestURL, Vector2{-200, 120}, Color::createBlack(), 0.0f, Vector2{0.0f, 0.5f}, 1.0f);
    spriteFont->draw(*spriteBatch, webText, Vector2::createZero(), Color::createWhite(), 0.0f, Vector2{0.5f, 0.5f}, 1.0f);

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
