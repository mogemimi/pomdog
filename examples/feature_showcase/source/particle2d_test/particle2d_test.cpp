#include "particle2d_test.h"
#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape_box.h"
#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape_sector.h"
#include "pomdog/experimental/particles/parameters/particle_parameter_constant.h"
#include "pomdog/experimental/particles/parameters/particle_parameter_curve.h"
#include "pomdog/experimental/particles/parameters/particle_parameter_random.h"
#include "pomdog/experimental/particles/parameters/particle_parameter_random_curves.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {
namespace {

std::shared_ptr<ParticleClip> CreateEmitterFireBlock()
{
    using detail::particles::ParticleCurveKey;
    using detail::particles::ParticleEmitterShapeBox;
    using detail::particles::ParticleParameterConstant;
    using detail::particles::ParticleParameterCurve;
    using detail::particles::ParticleParameterRandom;

    auto clip = std::make_shared<ParticleClip>();

    clip->MaxParticles = 1024;
    clip->EmissionRate = 128 * 2;
    clip->EmissionRateOverTime = 128 * 2;
    clip->Looping = true;
    clip->StartLifetime = 1.8f;
    clip->GravityModifier = 0.0f;

    clip->Duration = std::chrono::milliseconds(10);

#if 0
    clip->Shape = std::make_unique<ParticleEmitterShapeSector>(math::PiOver4<float>);
#else
    clip->Shape = std::make_unique<ParticleEmitterShapeBox>(Vector3{20.0f, 50.0f, 0.0f});
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
    clip->StartColor = std::make_unique<ParticleParameterConstant<Color>>(Color::createWhite());
#else
    clip->StartColor = std::make_unique<ParticleParameterRandom<Color>>(Color::createBlack(), Color{200, 180, 100, 50});
#endif

#if 0
    clip->ColorOverLifetime = std::make_unique<ParticleParameterConstant<Color>>(Color::createWhite());
#elif 0
    clip->ColorOverLifetime = std::make_unique<ParticleParameterRandom<Color>>(Color::createYellow(), Color::createBlack());
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
    clip->StartRotation = std::make_unique<ParticleParameterConstant<Radian<f32>>>(0.0f);
#else
    clip->StartRotation = std::make_unique<ParticleParameterRandom<Radian<f32>>>(
        0.0f, math::TwoPi<float>);
#endif

#if 0
    clip->Emitter.RotationOverLifetime = std::make_unique<ParticleParameterConstant<Radian<f32>>>(0);
#else
    clip->RotationOverLifetime = std::make_unique<ParticleParameterRandom<Radian<f32>>>(
        -math::PiOver4<float>, math::PiOver4<float>);
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
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error> Particle2DTest::initialize()
{
    auto assets = gameHost->getAssetManager();
    auto clock = gameHost->getClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->createCommandList();
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }

    primitiveBatch = std::make_shared<PrimitiveBatch>(graphicsDevice, *assets);
    spriteBatch = std::make_shared<SpriteBatch>(
        graphicsDevice,
        gpu::BlendDescriptor::createAlphaBlend(),
        std::nullopt,
        gpu::SamplerDescriptor::createLinearWrap(),
        std::nullopt,
        std::nullopt,
        SpriteBatchPixelShaderMode::Default,
        *assets);

    // NOTE: Load particle texture
    std::tie(texture, err) = assets->load<gpu::Texture2D>("Textures/particle_smoke.png");
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }

    timer = std::make_shared<Timer>(clock);
    timer->setInterval(std::chrono::seconds(1));
    timer->setScale(0.2);

    particleClip = CreateEmitterFireBlock();
    particleSystem = std::make_unique<ParticleSystem>(particleClip);
    particleSystem->Play();

    emitterPosition = Vector2::createZero();

    auto mouse = gameHost->getMouse();
    connect(mouse->Moved, [this](const Point2D& mousePos) {
        const auto mouse = gameHost->getMouse();
        const auto mouseState = mouse->getState();
        if (mouseState.leftButton != ButtonState::Pressed) {
            return;
        }
        const auto window = gameHost->getWindow();
        const auto clientBounds = window->getClientBounds();
        auto pos = mousePos;
        pos.x = pos.x - (clientBounds.width / 2);
        pos.y = -pos.y + (clientBounds.height / 2);
        emitterPosition = math::toVector2(pos);
    });

    return nullptr;
}

void Particle2DTest::update()
{
    auto clock = gameHost->getClock();
    auto frameDuration = clock->getFrameDuration();
    particleSystem->Simulate(emitterPosition, math::toRadian(90.0f), frameDuration);
}

void Particle2DTest::draw()
{
    auto presentationParameters = graphicsDevice->getPresentationParameters();

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

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<float>(presentationParameters.backBufferWidth),
        static_cast<float>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    // Drawing line
    const auto w = static_cast<float>(presentationParameters.backBufferWidth);
    const auto h = static_cast<float>(presentationParameters.backBufferHeight);
    primitiveBatch->begin(commandList, projectionMatrix);
    primitiveBatch->drawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->drawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->drawLine(Vector2{-w * 0.5f, h * 0.25f}, Vector2{w * 0.5f, h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->drawLine(Vector2{-w * 0.5f, -h * 0.25f}, Vector2{w * 0.5f, -h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->drawLine(Vector2{-w * 0.25f, -h * 0.5f}, Vector2{-w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->drawLine(Vector2{w * 0.25f, -h * 0.5f}, Vector2{w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->end();

    spriteBatch->begin(commandList, projectionMatrix);

    for (const auto& particle : particleSystem->GetParticles()) {
        spriteBatch->draw(
            texture,
            Vector2{particle.Position.x, particle.Position.y},
            Rectangle{0, 0, 64, 64},
            particle.Color,
            particle.Rotation,
            Vector2{0.5f, 0.5f},
            particle.Size);
    }

    spriteBatch->end();

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
