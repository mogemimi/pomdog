#include "SVGDecodeTest.hpp"
#include <Pomdog/Experimental/Image/SVGLoader.hpp>

namespace FeatureShowcase {

SVGDecodeTest::SVGDecodeTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

void SVGDecodeTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();
    commandList = std::make_shared<GraphicsCommandList>(*graphicsDevice);
    spriteBatch = std::make_shared<SpriteBatch>(graphicsDevice, *assets);

    auto [font, fontErr] = assets->Load<TrueTypeFont>("Fonts/NotoSans/NotoSans-Regular.ttf");
    if (fontErr != nullptr) {
        Log::Critical("Error", "failed to load a font file: " + fontErr->ToString());
    }

    spriteFont = std::make_shared<SpriteFont>(graphicsDevice, font, 24.0f, 24.0f);
    spriteFont->PrepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");

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
        auto filePath = PathHelper::Normalize(PathHelper::Join(assets->GetContentDirectory(), s));
        constexpr int canvasWidth = 24;
        constexpr int canvasHeight = 24;

        if (auto [res, err] = SVG::LoadTexture(graphicsDevice, filePath, canvasWidth, canvasHeight); err != nullptr) {
            Log::Verbose("failed to load texture: " + err->ToString());
        }
        else {
            textures.push_back(std::move(res));
        }
    }
}

void SVGDecodeTest::Update()
{
}

void SVGDecodeTest::Draw()
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
    constexpr float marginY = 32.0f;
    constexpr float startY = 210.0f;
    float posY = startY;
    for (auto& t : textures) {
        spriteBatch->Draw(t, Vector2{-100.0f, posY}, Color::White);
        posY = posY - marginY;
    }
    posY = startY;
    for (auto& t : svgFiles) {
        spriteFont->Draw(*spriteBatch, t, Vector2{-60.0f, posY}, Color::White, 0.0f, Vector2{0.0f, 0.3f}, 0.8f);
        posY = posY - marginY;
    }
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
