#include "GIFDecodeTest.hpp"
#include <pomdog/experimental/image/gif_image.hpp>
#include <pomdog/experimental/image/gif_loader.hpp>
#include <pomdog/experimental/texture_packer/texture_atlas_generator.hpp>
#include <pomdog/experimental/tween/easing_helper.hpp>
#include <cstdint>
#include <random>

namespace FeatureShowcase {

GIFDecodeTest::GIFDecodeTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

std::unique_ptr<Error> GIFDecodeTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->CreateGraphicsCommandList();
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to create graphics command list");
    }

    spriteBatch = std::make_shared<SpriteBatch>(
        graphicsDevice,
        BlendDescription::CreateAlphaBlend(),
        std::nullopt,
        SamplerDescription::CreatePointWrap(),
        std::nullopt,
        std::nullopt,
        SpriteBatchPixelShaderMode::Default,
        *assets);

    // NOTE: Loading GIF image
    auto [gif, gifErr] = GIF::DecodeFile(PathHelper::Join(assets->GetContentDirectory(), "Textures/punch.gif"));
    if (gifErr != nullptr) {
        return errors::Wrap(std::move(gifErr), "failed to load a gif file");
    }

    // NOTE: Generating texture atlas from GIF image
    std::vector<TexturePacker::TextureAtlasGeneratorSource> sources;
    for (auto& frame : gif.Frames) {
        TexturePacker::TextureAtlasGeneratorSource src;
        src.Image = frame.Image;
        src.Name = std::to_string(sources.size());
        sources.push_back(std::move(src));
    }
    auto result = TexturePacker::TextureAtlasGenerator::Generate(sources, 512, 512);
    POMDOG_ASSERT(result.Image != nullptr);

    // NOTE: Convert image to pre-multiplied alpha format
    result.Image->PremultiplyAlpha();

    // NOTE: Creating texture from packed image
    std::tie(texture, err) = graphicsDevice->CreateTexture2D(
        result.Image->GetWidth(),
        result.Image->GetHeight(),
        false,
        SurfaceFormat::R8G8B8A8_UNorm);
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to create texture from packed image");
    }
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
        std::uniform_int_distribution<int> colorDist(200, 255);

        const auto scale = scaleDist(random);
        const auto reverse = ((intDist(random) % 2 == 0) ? -1.0f : 1.0f);

        SpriteInstance sprite;
        sprite.Position = math::ToVector2(pos);
        sprite.Scale.X = scale * reverse;
        sprite.Scale.Y = scale;
        sprite.StartFrameIndex = intDist(random);
        sprite.Color.R = static_cast<std::uint8_t>(colorDist(random));
        sprite.Color.G = static_cast<std::uint8_t>(colorDist(random));
        sprite.Color.B = static_cast<std::uint8_t>(colorDist(random));
        sprite.Color.A = 255;
        sprites.push_back(std::move(sprite));
    });

    return nullptr;
}

void GIFDecodeTest::Update()
{
}

void GIFDecodeTest::Draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    RenderPass pass;
    pass.RenderTargets[0] = {nullptr, Color::CornflowerBlue().ToVector4()};
    pass.DepthStencilBuffer = nullptr;
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = static_cast<std::uint8_t>(0);
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        static_cast<float>(presentationParameters.BackBufferWidth),
        static_cast<float>(presentationParameters.BackBufferHeight),
        0.0f,
        100.0f);

    const auto t = static_cast<float>(timer->GetTotalTime().count());

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
            math::TwoPi<float> * Easings::EaseSine::InOut(t),
            Vector2{0.5f, 0.5f},
            sprite.Scale);
    }

    const auto& frame = textureAtlas.regions[currentFrameIndex];
    spriteBatch->Draw(
        texture,
        Vector2{64.0f, 64.0f},
        frame.Region,
        Color::White(),
        0.0f,
        Vector2{1.0f, 1.0f},
        2.0f);

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
