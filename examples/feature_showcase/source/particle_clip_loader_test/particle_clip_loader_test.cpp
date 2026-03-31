#include "particle_clip_loader_test.h"
#include "pomdog/experimental/particles/particle_clip_loader.h"
#include "pomdog/utility/path_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

ParticleClipLoaderTest::ParticleClipLoaderTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
ParticleClipLoaderTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/, int /*argc*/, const char* const* /*argv*/)
{
    auto clock = gameHost_->getClock();

    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    if (auto [p, err] = createPrimitivePipeline(fs_, graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create PrimitivePipeline");
    }
    else {
        primitivePipeline_ = std::move(p);
    }
    if (auto [p, err] = createPrimitiveBatch(graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create PrimitiveBatch");
    }
    else {
        primitiveBatch_ = std::move(p);
    }
    if (auto [p, err] = createSpritePipeline(
            fs_,
            graphicsDevice_,
            gpu::BlendDesc::createAlphaBlend(),
            std::nullopt,
            gpu::SamplerDesc::createPointWrap(),
            std::nullopt,
            std::nullopt,
            SpriteBatchPixelShaderMode::Default);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpritePipeline");
    }
    else {
        spritePipeline_ = std::move(p);
    }
    if (auto [p, err] = createSpriteBatch(graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteBatch");
    }
    else {
        spriteBatch_ = std::move(p);
    }

    // NOTE: Load particle texture.
    if (auto [texture, err] = loadTexture2D(fs_, graphicsDevice_, "/assets/textures/particle_smoke.png"); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }
    else {
        texture_ = std::move(texture);
    }

    timer_ = std::make_shared<Timer>(clock);
    timer_->setInterval(std::chrono::seconds(1));
    timer_->setScale(0.2);

    {
        // NOTE: Load particle clip from .json file
        auto [particleClip, clipErr] = loadParticleClip(fs_, "/assets/particles/fire2d.json");
        if (clipErr != nullptr) {
            return errors::wrap(std::move(clipErr), "failed to load particle json");
        }

        particleSystem_ = std::make_unique<ParticleSystem>(particleClip);
        particleSystem_->Play();
    }

    emitterPosition_ = Vector2::createZero();

    auto mouse = gameHost_->getMouse();
    auto onMoved = [this](const Point2D& mousePos) {
        const auto mouse = gameHost_->getMouse();
        const auto mouseState = mouse->getState();
        if (mouseState.leftButton != ButtonState::Down) {
            return;
        }
        const auto window = gameHost_->getWindow();
        const auto clientBounds = window->getClientBounds();
        auto pos = mousePos;
        pos.x = pos.x - (clientBounds.width / 2);
        pos.y = -pos.y + (clientBounds.height / 2);
        emitterPosition_ = math::toVector2(pos);
    };
    auto onClipChanged = [this] {
        std::array<std::string, 2> filenames = {
            "particles/fire2d.json",
            "particles/water2d.json",
        };

        currentClipIndex_++;
        if (currentClipIndex_ >= filenames.size()) {
            currentClipIndex_ = 0;
        }

        // NOTE: Load particle clip from .json file
        auto [particleClip, clipErr] = loadParticleClip(fs_, filepaths::joinUnix("/assets", filenames[currentClipIndex_]));
        if (clipErr != nullptr) {
            Log::Verbose("failed to load particle json: " + clipErr->toString());
        }
        else {
            particleSystem_ = std::make_unique<ParticleSystem>(particleClip);
        }
    };
    connect_(mouse->ButtonDown, [this, onMoved, onClipChanged]([[maybe_unused]] MouseButtons mouseButton) {
        auto mouse = gameHost_->getMouse();
        auto mouseState = mouse->getState();
        if (mouseState.leftButton == ButtonState::Down) {
            onMoved(mouseState.position);
        }
        if (mouseState.rightButton == ButtonState::Down) {
            onClipChanged();
        }
    });
    connect_(mouse->Moved, onMoved);

    return nullptr;
}

void ParticleClipLoaderTest::update()
{
    auto clock = gameHost_->getClock();
    auto frameDuration = clock->getFrameDuration();
    particleSystem_->Simulate(emitterPosition_, math::toRadian(90.0f), frameDuration);
}

void ParticleClipLoaderTest::draw()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = std::uint8_t(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<float>(presentationParameters.backBufferWidth),
        static_cast<float>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    // Drawing line
    const auto w = static_cast<float>(presentationParameters.backBufferWidth);
    const auto h = static_cast<float>(presentationParameters.backBufferHeight);
    primitiveBatch_->begin(commandList_, primitivePipeline_, projectionMatrix);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch_->drawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, h * 0.25f}, Vector2{w * 0.5f, h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, -h * 0.25f}, Vector2{w * 0.5f, -h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->drawLine(Vector2{-w * 0.25f, -h * 0.5f}, Vector2{-w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->drawLine(Vector2{w * 0.25f, -h * 0.5f}, Vector2{w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->end();

    spriteBatch_->begin(commandList_, spritePipeline_, projectionMatrix);

    for (const auto& particle : particleSystem_->GetParticles()) {
        spriteBatch_->draw(
            texture_,
            Vector2{particle.Position.x, particle.Position.y},
            Rect2D{0, 0, 64, 64},
            particle.Color,
            particle.Rotation,
            Vector2{0.5f, 0.5f},
            particle.Size);
    }

    spriteBatch_->end();

    commandList_->endRenderPass();
    commandList_->close();

    constexpr bool isStandalone = false;
    if constexpr (isStandalone) {
        commandQueue_->reset();
        commandQueue_->pushBackCommandList(commandList_);
        commandQueue_->executeCommandLists();
        commandQueue_->present();
    }
    else {
        commandQueue_->pushBackCommandList(commandList_);
    }
}

} // namespace feature_showcase
