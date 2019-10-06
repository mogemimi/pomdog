#include "Particle2DTest.hpp"
#include <Pomdog/Experimental/Particle2D/detail/ParticleEmitterShapeBox.hpp>
#include <Pomdog/Experimental/Particle2D/detail/ParticleEmitterShapeSector.hpp>
#include <Pomdog/Experimental/Particle2D/detail/ParticleParameterConstant.hpp>
#include <Pomdog/Experimental/Particle2D/detail/ParticleParameterCurve.hpp>
#include <Pomdog/Experimental/Particle2D/detail/ParticleParameterRandom.hpp>
#include <Pomdog/Experimental/Particle2D/detail/ParticleParameterRandomCurves.hpp>
#include <random>

namespace FeatureShowcase {
namespace {

std::shared_ptr<ParticleClip> CreateEmitterFireBlock()
{
    using Detail::Particles::ParticleCurveKey;
    using Detail::Particles::ParticleEmitterShapeBox;
    using Detail::Particles::ParticleParameterConstant;
    using Detail::Particles::ParticleParameterCurve;
    using Detail::Particles::ParticleParameterRandom;

    auto clip = std::make_shared<ParticleClip>();

    clip->Emitter.MaxParticles = 1024;
    clip->Emitter.EmissionRate = 128 * 2;
    // clip->Emitter.Looping = false;
    clip->Emitter.StartLifetime = 1.8f;
    // clip->Emitter.GravityModifier = 100.0f;

    clip->Duration = std::chrono::milliseconds(10);

#if 0
    clip->Shape = std::make_unique<ParticleEmitterShapeSector>(Math::PiOver4<float>);
#else
    clip->Shape = std::make_unique<ParticleEmitterShapeBox>(20.0f, 50.0f);
#endif

#if 1
    clip->StartSpeed = std::make_unique<ParticleParameterRandom<float>>(40.0f, 128.0f);
#elif 1
    clip->StartSpeed = std::make_unique<ParticleParameterConstant<float>>(-128.0f);
#else
    clip->StartSpeed = std::make_unique<ParticleParameterCurve<float>>(
        std::initializer_list<ParticleCurveKey<float>>{
            {0.00f, 0.0f},
            {0.10f, -0.5f},
            {0.15f, -1.0f},
            {0.20f, -0.5f},
            {0.40f, 0.0f},
            {0.50f, 0.5f},
            {0.70f, 1.0f},
            {0.80f, 0.5f},
            {1.00f, 0.0f},
        });
#endif

#if 0
    clip->StartColor = std::make_unique<ParticleParameterConstant<Color>>(Color::White);
#else
    clip->StartColor = std::make_unique<ParticleParameterRandom<Color>>(Color::Black, Color{200, 180, 100, 50});
#endif

#if 0
    clip->ColorOverLifetime = std::make_unique<ParticleParameterConstant<Color>>(Color::White);
#elif 0
    clip->ColorOverLifetime = std::make_unique<ParticleParameterRandom<Color>>(Color::Yellow, Color::Black);
#else
    clip->ColorOverLifetime = std::make_unique<ParticleParameterCurve<Color>>(
        std::initializer_list<ParticleCurveKey<Color>>{
            {0.00f, Color{230, 220, 190, 0}},
            {0.02f, Color{255, 255, 180, 10}},
            {0.09f, Color{255, 250, 170, 100}},
            {0.15f, Color{255, 200, 160, 130}},
            {0.19f, Color{200, 130, 60, 255}},
            {0.24f, Color{190, 50, 10, 80}},
            {0.32f, Color{80, 24, 2, 20}},
            {1.00f, Color{0, 0, 0, 0}},
        });
#endif

#if 0
    clip->StartRotation = std::make_unique<ParticleParameterConstant<Radian<float>>>(0.0f);
#else
    clip->StartRotation = std::make_unique<ParticleParameterRandom<Radian<float>>>(
        0.0f, Math::TwoPi<float>);
#endif

#if 0
    clip->Emitter.RotationOverLifetime = std::make_unique<ParticleParameterConstant<Radian<float>>>(0);
#else
    clip->RotationOverLifetime = std::make_unique<ParticleParameterRandom<Radian<float>>>(
        -Math::PiOver4<float>, Math::PiOver4<float>);
#endif

#if 0
    clip->StartSize = std::make_unique<ParticleParameterConstant<float>>(1.0f);
#else
    clip->StartSize = std::make_unique<ParticleParameterRandom<float>>(0.8f, 1.2f);
#endif

#if 0
    clip->SizeOverLifetime = std::make_unique<ParticleParameterConstant<float>>(1.0f);
#else
    clip->SizeOverLifetime = std::make_unique<ParticleParameterCurve<float>>(
        std::initializer_list<ParticleCurveKey<float>>{
            {0.00f, 0.0f},
            {0.03f, 0.5f},
            {0.10f, 0.8f},
            {0.15f, 1.0f},
            {0.60f, 0.8f},
            {1.00f, 0.0f},
        });
#endif

    return clip;
}

} // namespace

Particle2DTest::Particle2DTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

void Particle2DTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();
    commandList = std::make_shared<GraphicsCommandList>(*graphicsDevice);
    primitiveBatch = std::make_shared<PrimitiveBatch>(graphicsDevice, *assets);
    spriteBatch = std::make_shared<SpriteBatch>(
        graphicsDevice,
        BlendDescription::CreateAlphaBlend(),
        SamplerDescription::CreatePointWrap(),
        std::nullopt,
        std::nullopt,
        *assets);

    std::shared_ptr<Error> err;
    std::tie(texture, err) = assets->Load<Texture2D>("Textures/particle_smoke.png");
    if (err != nullptr) {
        Log::Verbose("failed to load texture: " + err->ToString());
    }

    timer = std::make_shared<Timer>(clock);
    timer->SetInterval(std::chrono::seconds(1));
    timer->SetScale(0.2);

    particleClip = CreateEmitterFireBlock();
    particleSystem = std::make_unique<ParticleSystem>(particleClip);
    particleSystem->Play();

    emitterPosition = Vector2::Zero;

    auto mouse = gameHost->GetMouse();
    connect(mouse->Moved, [this](const Point2D& mousePos) {
        auto mouse = gameHost->GetMouse();
        auto mouseState = mouse->GetState();
        if (mouseState.LeftButton != ButtonState::Pressed) {
            return;
        }
        auto window = gameHost->GetWindow();
        auto pos = mousePos;
        pos.X = pos.X - (window->GetClientBounds().Width / 2);
        pos.Y = -pos.Y + (window->GetClientBounds().Height / 2);
        emitterPosition = MathHelper::ToVector2(pos);
    });
}

void Particle2DTest::Update()
{
    auto clock = gameHost->GetClock();
    auto frameDuration = clock->GetFrameDuration();
    particleSystem->Simulate(emitterPosition, MathHelper::ToRadians(90.0f), frameDuration);
}

void Particle2DTest::Draw()
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
            Vector2{particle.Position.X, particle.Position.Y},
            Rectangle{0, 0, 64, 64},
            particle.Color,
            particle.Rotation,
            Vector2{0.5f, 0.5f},
            particle.Size);
    }

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
