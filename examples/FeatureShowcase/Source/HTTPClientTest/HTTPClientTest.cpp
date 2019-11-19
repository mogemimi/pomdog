#include "HTTPClientTest.hpp"

namespace FeatureShowcase {

HTTPClientTest::HTTPClientTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

void HTTPClientTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();
    commandList = std::make_shared<GraphicsCommandList>(*graphicsDevice);
    spriteBatch = std::make_shared<SpriteBatch>(graphicsDevice, *assets);

    auto[font, fontErr] = assets->Load<TrueTypeFont>("Fonts/NotoSans/NotoSans-Regular.ttf");
    if (fontErr != nullptr) {
        Log::Critical("Error", "failed to load a font file: " + fontErr->ToString());
    }

    spriteFont = std::make_shared<SpriteFont>(graphicsDevice, font, 24.0f, 24.0f);
    spriteFont->PrepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");

    auto http = gameHost->GetHTTPClient();
    auto callback = [this](const std::shared_ptr<HTTPResponse>& resp, const std::shared_ptr<Error>& err) {
        if (err != nullptr) {
            webText = err->ToString();
            return;
        }
        webText = std::string{resp->Body.data() , resp->Body.size()};
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
    auto[conn, err] = http->Get("https://www.google.com/humans.txt", std::move(callback));
    if (err != nullptr) {
        webText = err->ToString();
    }
    connect += std::move(conn);
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
    pass.RenderTargets[0] = {nullptr, Color::CornflowerBlue.ToVector4()};
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = 0;
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    spriteBatch->Begin(commandList, projectionMatrix);
    spriteFont->Draw(*spriteBatch, requestURL, Vector2{-200, 120}, Color::Black, 0.0f, Vector2{ 0.0f, 0.5f }, 1.0f);
    spriteFont->Draw(*spriteBatch, webText, Vector2::Zero, Color::White, 0.0f, Vector2{0.5f, 0.5f}, 1.0f);

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

} // namespace FeatureShowcase
