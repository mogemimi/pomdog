#include "Texture2DLoaderTest.hpp"

namespace FeatureShowcase {

Texture2DLoaderTest::Texture2DLoaderTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

void Texture2DLoaderTest::Initialize()
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

    // NOTE: Load PNG texture.
    if (auto[res, err] = assets->Load<Texture2D>("Textures/pomdog.png"); err != nullptr) {
        Log::Verbose("failed to load texture: " + err->ToString());
    }
    else {
        texturePNG = std::move(res);
    }

    // NOTE: Load DDS texture (DXT1 compression).
    if (auto[res, err] = assets->Load<Texture2D>("Textures/pomdog-dxt1.dds"); err != nullptr) {
        Log::Verbose("failed to load texture: " + err->ToString());
    }
    else {
        textureDXT1 = std::move(res);
    }

    // NOTE: Load DDS texture (DXT5 compression).
    if (auto[res, err] = assets->Load<Texture2D>("Textures/pomdog-dxt5.dds"); err != nullptr) {
        Log::Verbose("failed to load texture: " + err->ToString());
    }
    else {
        textureDXT5 = std::move(res);
    }

    // NOTE: Load PNM/Netpbm bitmap ascii texture (P1).
    if (auto[res, err] = assets->Load<Texture2D>("Textures/pomdog-p1.pbm"); err != nullptr) {
        Log::Verbose("failed to load texture: " + err->ToString());
    }
    else {
        texturePNMP1 = std::move(res);
    }

    // NOTE: Load PNM/Netpbm graymap ascii texture (P2).
    if (auto[res, err] = assets->Load<Texture2D>("Textures/pomdog-p2.pgm"); err != nullptr) {
        Log::Verbose("failed to load texture: " + err->ToString());
    }
    else {
        texturePNMP2 = std::move(res);
    }

    // NOTE: Load PNM/Netpbm pixmap ascii texture (P3).
    if (auto[res, err] = assets->Load<Texture2D>("Textures/pomdog-p3.ppm"); err != nullptr) {
        Log::Verbose("failed to load texture: " + err->ToString());
    }
    else {
        texturePNMP3 = std::move(res);
    }

    // NOTE: Load PNM/Netpbm bitmap binary texture (P4).
    if (auto[res, err] = assets->Load<Texture2D>("Textures/pomdog-p4.pbm"); err != nullptr) {
        Log::Verbose("failed to load texture: " + err->ToString());
    }
    else {
        texturePNMP4 = std::move(res);
    }

    // NOTE: Load PNM/Netpbm graymap binary texture (P5).
    if (auto[res, err] = assets->Load<Texture2D>("Textures/pomdog-p5.pgm"); err != nullptr) {
        Log::Verbose("failed to load texture: " + err->ToString());
    }
    else {
        texturePNMP5 = std::move(res);
    }

    // NOTE: Load PNM/Netpbm pixmap binary texture (P6).
    if (auto[res, err] = assets->Load<Texture2D>("Textures/pomdog-p6.ppm"); err != nullptr) {
        Log::Verbose("failed to load texture: " + err->ToString());
    }
    else {
        texturePNMP6 = std::move(res);
    }
}

void Texture2DLoaderTest::Update()
{
}

void Texture2DLoaderTest::Draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        static_cast<float>(presentationParameters.BackBufferWidth),
        static_cast<float>(presentationParameters.BackBufferHeight),
        0.0f,
        100.0f);

    Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    RenderPass pass;
    pass.RenderTargets.emplace_back(nullptr, Color::CornflowerBlue.ToVector4());
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = 0;
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

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

    spriteBatch->Begin(commandList, projectionMatrix);
    constexpr float marginY = 42.0f;
    constexpr float startY = 160.0f;
    float posY = startY;
    for (auto& t : textures) {
        spriteBatch->Draw(t, Vector2{-100.0f, posY}, Color::White);
        posY = posY - marginY;
    }
    posY = startY;
    for (auto& t : texts) {
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
