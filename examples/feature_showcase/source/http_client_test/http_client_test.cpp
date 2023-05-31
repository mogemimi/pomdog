#include "http_client_test.h"

namespace feature_showcase {

HTTPClientTest::HTTPClientTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error> HTTPClientTest::initialize()
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

    auto [font, fontErr] = assets->Load<TrueTypeFont>("Fonts/NotoSans/NotoSans-Regular.ttf");
    if (fontErr != nullptr) {
        return errors::wrap(std::move(fontErr), "failed to load a font file");
    }

    spriteFont = std::make_shared<SpriteFont>(graphicsDevice, font, 24.0f, 24.0f);
    spriteFont->PrepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");

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
    commandList->setRenderPass(std::move(pass));

    spriteBatch->Begin(commandList, projectionMatrix);
    spriteFont->Draw(*spriteBatch, requestURL, Vector2{-200, 120}, Color::createBlack(), 0.0f, Vector2{0.0f, 0.5f}, 1.0f);
    spriteFont->Draw(*spriteBatch, webText, Vector2::createZero(), Color::createWhite(), 0.0f, Vector2{0.5f, 0.5f}, 1.0f);

    spriteBatch->End();

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
