#include "particle_clip_loader_test.h"
#include "pomdog/experimental/particles/particle_clip_loader.h"
#include <random>

namespace feature_showcase {

ParticleClipLoaderTest::ParticleClipLoaderTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetCommandQueue())
{
}

std::unique_ptr<Error> ParticleClipLoaderTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->CreateCommandList();
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to create graphics command list");
    }

    primitiveBatch = std::make_shared<PrimitiveBatch>(graphicsDevice, *assets);
    spriteBatch = std::make_shared<SpriteBatch>(
        graphicsDevice,
        gpu::BlendDescriptor::CreateAlphaBlend(),
        std::nullopt,
        gpu::SamplerDescriptor::CreatePointWrap(),
        std::nullopt,
        std::nullopt,
        SpriteBatchPixelShaderMode::Default,
        *assets);

    // NOTE: Load particle texture.
    std::tie(texture, err) = assets->Load<gpu::Texture2D>("Textures/particle_smoke.png");
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to load texture");
    }

    timer = std::make_shared<Timer>(clock);
    timer->SetInterval(std::chrono::seconds(1));
    timer->SetScale(0.2);

    {
        // NOTE: Load particle clip from .json file
        auto [particleClip, clipErr] = assets->Load<ParticleClip>("Particles/Fire2D.json");
        if (clipErr != nullptr) {
            return errors::Wrap(std::move(err), "failed to load particle json");
        }

        particleSystem = std::make_unique<ParticleSystem>(particleClip);
        particleSystem->Play();
    }

    emitterPosition = Vector2::Zero();

    auto mouse = gameHost->GetMouse();
    auto onMoved = [this](const Point2D& mousePos) {
        const auto mouse = gameHost->GetMouse();
        const auto mouseState = mouse->GetState();
        if (mouseState.LeftButton != ButtonState::Pressed) {
            return;
        }
        const auto window = gameHost->GetWindow();
        const auto clientBounds = window->GetClientBounds();
        auto pos = mousePos;
        pos.x = pos.x - (clientBounds.width / 2);
        pos.y = -pos.y + (clientBounds.height / 2);
        emitterPosition = math::ToVector2(pos);
    };
    auto onClipChanged = [this] {
        std::array<std::string, 2> filenames = {
            "Particles/Fire2D.json",
            "Particles/Water2D.json",
        };

        currentClipIndex++;
        if (currentClipIndex >= filenames.size()) {
            currentClipIndex = 0;
        }

        // NOTE: Load particle clip from .json file
        auto assets = gameHost->GetAssetManager();
        auto [particleClip, clipErr] = assets->Load<ParticleClip>(filenames[currentClipIndex]);
        if (clipErr != nullptr) {
            Log::Verbose("failed to load particle json: " + clipErr->ToString());
        }
        else {
            particleSystem = std::make_unique<ParticleSystem>(particleClip);
        }
    };
    connect(mouse->ButtonDown, [this, onMoved, onClipChanged]([[maybe_unused]] MouseButtons mouseButton) {
        auto mouse = gameHost->GetMouse();
        auto mouseState = mouse->GetState();
        if (mouseState.LeftButton == ButtonState::Pressed) {
            onMoved(mouseState.Position);
        }
        if (mouseState.RightButton == ButtonState::Pressed) {
            onClipChanged();
        }
    });
    connect(mouse->Moved, onMoved);

    return nullptr;
}

void ParticleClipLoaderTest::Update()
{
    auto clock = gameHost->GetClock();
    auto frameDuration = clock->GetFrameDuration();
    particleSystem->Simulate(emitterPosition, math::ToRadians(90.0f), frameDuration);
}

void ParticleClipLoaderTest::Draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    gpu::RenderPass pass;
    pass.RenderTargets[0] = {nullptr, Color::CornflowerBlue().ToVector4()};
    pass.DepthStencilBuffer = nullptr;
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = std::uint8_t(0);
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        static_cast<float>(presentationParameters.BackBufferWidth),
        static_cast<float>(presentationParameters.BackBufferHeight),
        0.0f,
        100.0f);

    // Drawing line
    const auto w = static_cast<float>(presentationParameters.BackBufferWidth);
    const auto h = static_cast<float>(presentationParameters.BackBufferHeight);
    primitiveBatch->Begin(commandList, projectionMatrix);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->DrawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, h * 0.25f}, Vector2{w * 0.5f, h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, -h * 0.25f}, Vector2{w * 0.5f, -h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.25f, -h * 0.5f}, Vector2{-w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{w * 0.25f, -h * 0.5f}, Vector2{w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->End();

    spriteBatch->Begin(commandList, projectionMatrix);

    for (const auto& particle : particleSystem->GetParticles()) {
        spriteBatch->Draw(
            texture,
            Vector2{particle.Position.x, particle.Position.y},
            Rectangle{0, 0, 64, 64},
            particle.Color,
            particle.Rotation,
            Vector2{0.5f, 0.5f},
            particle.Size);
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

} // namespace feature_showcase
