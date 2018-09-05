#include "SpriteBatchTest.hpp"
#include <Pomdog/Experimental/Image/GifImage.hpp>
#include <Pomdog/Experimental/Image/GifImageLoader.hpp>
#include <Pomdog/Experimental/TexturePacker/TextureAtlasGenerator.hpp>
#include <Pomdog/Experimental/Tween/EasingHelper.hpp>
#include <random>

namespace FeatureShowcase {

SpriteBatchTest::SpriteBatchTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

void SpriteBatchTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();
    commandList = std::make_shared<GraphicsCommandList>(*graphicsDevice);
    polygonBatch = std::make_shared<PolygonBatch>(graphicsDevice, *assets);
    spriteBatch = std::make_shared<SpriteBatch>(
        graphicsDevice,
        BlendDescription::CreateAlphaBlend(),
        SamplerDescription::CreatePointWrap(),
        std::nullopt,
        std::nullopt,
        *assets);

    texture2 = assets->Load<Texture2D>("Textures/pomdog.png");

    // Loading GIF image
    auto gif = GifLoader::Open(PathHelper::Join(assets->GetRootDirectory(), "Textures/punch.gif"));
    POMDOG_ASSERT(gif);

    // Generating texture atlas from GIF image
    std::vector<TexturePacker::TextureAtlasGeneratorSource> sources;
    for (auto& frame : gif->Frames) {
        TexturePacker::TextureAtlasGeneratorSource src;
        src.Image = frame.Image;
        src.Name = std::to_string(sources.size());
        sources.push_back(std::move(src));
    }
    auto result = TexturePacker::TextureAtlasGenerator::Generate(sources, 512, 512);
    POMDOG_ASSERT(result.Image);

    // Convert image to pre-multiplied alpha format
    result.Image->PremultiplyAlpha();

    // Creating texture from packed image
    texture = std::make_shared<Texture2D>(
        graphicsDevice,
        result.Image->GetWidth(),
        result.Image->GetHeight(),
        false,
        SurfaceFormat::R8G8B8A8_UNorm);
    texture->SetData(result.Image->GetData());

    textureAtlas = std::move(result.Atlas);

    timer = std::make_shared<Timer>(clock);
    timer->SetInterval(std::chrono::seconds(1));
    timer->SetScale(0.2);

    animationTimer = std::make_shared<Timer>(clock);
    animationTimer->SetInterval(std::chrono::milliseconds(100));
    currentFrameIndex = 0;
    connect(animationTimer->Elapsed, [this] {
        currentFrameIndex++;
        if (currentFrameIndex >= static_cast<int>(textureAtlas.regions.size())) {
            currentFrameIndex = 0;
        }
    });

    auto mouse = gameHost->GetMouse();
    connect(mouse->ButtonDown, [this](MouseButtons mouseButton) {
        if (mouseButton != MouseButtons::Left) {
            return;
        }
        auto window = gameHost->GetWindow();
        auto mouse = gameHost->GetMouse();
        auto mouseState = mouse->GetState();
        auto pos = mouseState.Position;
        pos.X = pos.X - (window->GetClientBounds().Width / 2);
        pos.Y = -pos.Y + (window->GetClientBounds().Height / 2);

        std::mt19937 random(std::random_device{}());
        std::uniform_real_distribution<float> scaleDist(1.0f, 2.0f);
        std::uniform_int_distribution<int> intDist(0, static_cast<int>(textureAtlas.regions.size()) - 1);
        std::uniform_int_distribution<uint8_t> colorDist(200, 255);

        const auto scale = scaleDist(random);
        const auto reverse = ((intDist(random) % 2 == 0) ? -1.0f : 1.0f);

        SpriteInstance sprite;
        sprite.Position = MathHelper::ToVector2(pos);
        sprite.Scale.X = scale * reverse;
        sprite.Scale.Y = scale;
        sprite.StartFrameIndex = intDist(random);
        sprite.Color.R = colorDist(random);
        sprite.Color.G = colorDist(random);
        sprite.Color.B = colorDist(random);
        sprite.Color.A = 255;
        sprites.push_back(std::move(sprite));
    });
}

void SpriteBatchTest::Update()
{
}

void SpriteBatchTest::Draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    RenderPass pass;
    pass.RenderTargets.emplace_back(nullptr, Color::CornflowerBlue.ToVector4());
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = 0;
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));
    commandList->Close();

    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        presentationParameters.BackBufferWidth,
        presentationParameters.BackBufferHeight,
        0.0f,
        100.0f);

    const auto t = static_cast<float>(timer->GetTotalTime().count());

    // Drawing line
    const auto w = static_cast<float>(presentationParameters.BackBufferWidth);
    const auto h = static_cast<float>(presentationParameters.BackBufferHeight);
    polygonBatch->Begin(commandList, projectionMatrix);
    polygonBatch->DrawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    polygonBatch->DrawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    polygonBatch->DrawLine(Vector2{-w * 0.5f, h * 0.25f}, Vector2{w * 0.5f, h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    polygonBatch->DrawLine(Vector2{-w * 0.5f, -h * 0.25f}, Vector2{w * 0.5f, -h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    polygonBatch->DrawLine(Vector2{-w * 0.25f, -h * 0.5f}, Vector2{-w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    polygonBatch->DrawLine(Vector2{w * 0.25f, -h * 0.5f}, Vector2{w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    polygonBatch->End();

    spriteBatch->Begin(commandList, projectionMatrix);

    for (const auto& sprite : sprites) {
        auto frameIndex = currentFrameIndex + sprite.StartFrameIndex;
        while (frameIndex >= static_cast<int>(textureAtlas.regions.size())) {
            frameIndex -= static_cast<int>(textureAtlas.regions.size());
        }

        const auto& frame = textureAtlas.regions[frameIndex];

        spriteBatch->Draw(
            texture,
            sprite.Position,
            frame.Region,
            sprite.Color,
            Math::TwoPi<float> * Easings::EaseSine::InOut(t),
            Vector2{0.5f, 0.5f},
            sprite.Scale);
    }

    spriteBatch->Draw(
        texture2,
        Vector2{64.0f, 64.0f},
        Rectangle{0, 0, 32, 32},
        Color::White,
        Math::TwoPi<float> * Easings::EaseSine::InOut(t),
        Vector2{1.0f, 1.0f},
        2.0f);

    const auto& frame = textureAtlas.regions[currentFrameIndex];
    spriteBatch->Draw(
        texture,
        Vector2{64.0f, 64.0f},
        frame.Region,
        Color::White,
        0.0f,
        Vector2{1.0f, 1.0f},
        2.0f);

    spriteBatch->End();

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
