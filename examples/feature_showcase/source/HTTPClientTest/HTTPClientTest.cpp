#include "HTTPClientTest.hpp"

namespace feature_showcase {

HTTPClientTest::HTTPClientTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

std::unique_ptr<Error> HTTPClientTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->CreateGraphicsCommandList();
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to create graphics command list");
    }

    spriteBatch = std::make_shared<SpriteBatch>(graphicsDevice, *assets);

    auto [font, fontErr] = assets->Load<TrueTypeFont>("Fonts/NotoSans/NotoSans-Regular.ttf");
    if (fontErr != nullptr) {
        return errors::Wrap(std::move(fontErr), "failed to load a font file");
    }

    spriteFont = std::make_shared<SpriteFont>(graphicsDevice, font, 24.0f, 24.0f);
    spriteFont->PrepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");

    auto http = gameHost->GetHTTPClient();
    auto callback = [this](const std::shared_ptr<HTTPResponse>& resp, const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            webText = err->ToString();
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
        webText = connErr->ToString();
    }
    connect += std::move(conn);

    return nullptr;
}

void HTTPClientTest::Update()
{
}

void HTTPClientTest::Draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        static_cast<float>(presentationParameters.BackBufferWidth),
        static_cast<float>(presentationParameters.BackBufferHeight),
        0.0f,
        100.0f);

    Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    RenderPass pass;
    pass.RenderTargets[0] = {nullptr, Color::CornflowerBlue().ToVector4()};
    pass.DepthStencilBuffer = nullptr;
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = std::uint8_t(0);
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    spriteBatch->Begin(commandList, projectionMatrix);
    spriteFont->Draw(*spriteBatch, requestURL, Vector2{-200, 120}, Color::Black(), 0.0f, Vector2{0.0f, 0.5f}, 1.0f);
    spriteFont->Draw(*spriteBatch, webText, Vector2::Zero(), Color::White(), 0.0f, Vector2{0.5f, 0.5f}, 1.0f);

    spriteBatch->End();

    commandList->Close();

    constexpr bool isStandalone = false;
    if constexpr (isStandalone) {
        commandQueue->Reset();
        commandQueue->PushbackCommandList(commandList);
        commandQueue->ExecuteCommandLists();
        commandQueue->Present();
    }
    else {
        commandQueue->PushbackCommandList(commandList);
    }
}

} // namespace feature_showcase
