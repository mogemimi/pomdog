#include "particle_clip_loader_test.h"
#include "pomdog/experimental/particles/particle_clip_loader.h"
#include <random>

namespace feature_showcase {

ParticleClipLoaderTest::ParticleClipLoaderTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error> ParticleClipLoaderTest::initialize()
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
        gpu::SamplerDescriptor::createPointWrap(),
        std::nullopt,
        std::nullopt,
        SpriteBatchPixelShaderMode::Default,
        *assets);

    // NOTE: Load particle texture.
    std::tie(texture, err) = assets->load<gpu::Texture2D>("Textures/particle_smoke.png");
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }

    timer = std::make_shared<Timer>(clock);
    timer->setInterval(std::chrono::seconds(1));
    timer->setScale(0.2);

    {
        // NOTE: Load particle clip from .json file
        auto [particleClip, clipErr] = assets->load<ParticleClip>("Particles/Fire2D.json");
        if (clipErr != nullptr) {
            return errors::wrap(std::move(err), "failed to load particle json");
        }

        particleSystem = std::make_unique<ParticleSystem>(particleClip);
        particleSystem->Play();
    }

    emitterPosition = Vector2::createZero();

    auto mouse = gameHost->getMouse();
    auto onMoved = [this](const Point2D& mousePos) {
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
        auto assets = gameHost->getAssetManager();
        auto [particleClip, clipErr] = assets->load<ParticleClip>(filenames[currentClipIndex]);
        if (clipErr != nullptr) {
            Log::Verbose("failed to load particle json: " + clipErr->toString());
        }
        else {
            particleSystem = std::make_unique<ParticleSystem>(particleClip);
        }
    };
    connect(mouse->ButtonDown, [this, onMoved, onClipChanged]([[maybe_unused]] MouseButtons mouseButton) {
        auto mouse = gameHost->getMouse();
        auto mouseState = mouse->getState();
        if (mouseState.leftButton == ButtonState::Pressed) {
            onMoved(mouseState.position);
        }
        if (mouseState.rightButton == ButtonState::Pressed) {
            onClipChanged();
        }
    });
    connect(mouse->Moved, onMoved);

    return nullptr;
}

void ParticleClipLoaderTest::update()
{
    auto clock = gameHost->getClock();
    auto frameDuration = clock->getFrameDuration();
    particleSystem->Simulate(emitterPosition, math::toRadians(90.0f), frameDuration);
}

void ParticleClipLoaderTest::draw()
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
