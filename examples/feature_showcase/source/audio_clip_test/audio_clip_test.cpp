#include "audio_clip_test.h"

namespace feature_showcase {

AudioClipTest::AudioClipTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
AudioClipTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/, int /*argc*/, const char* const* /*argv*/)
{
    auto clock = gameHost_->getClock();

    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    if (auto [p, err] = createSpritePipeline(
            fs_,
            graphicsDevice_,
            std::nullopt,
            std::nullopt,
            gpu::SamplerDesc::createLinearClamp(),
            std::nullopt,
            std::nullopt,
            SpriteBatchPixelShaderMode::DistanceField);
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

    auto [font, fontErr] = loadTrueTypeFont(fs_, "/assets/fonts/NotoSans-Regular.ttf");
    if (fontErr != nullptr) {
        return errors::wrap(std::move(fontErr), "failed to load a font file");
    }

    constexpr bool useSDF = true;

    if (auto [p, err] = createSpriteFont(graphicsDevice_, font, 24.0f, 24.0f, useSDF); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteFont");
    }
    else {
        spriteFont_ = std::move(p);
    }

    auto audioEngine = gameHost_->getAudioEngine();

    // NOTE: Load .wav audio file.
    if (auto [audioClip, clipErr] = loadAudioClip(fs_, gameHost_->getAudioEngine(), "/assets/sounds/pong1.wav"); clipErr != nullptr) {
        return errors::wrap(std::move(clipErr), "failed to load audio");
    }
    else {
        constexpr bool isLooped = false;
        if (auto [soundEffect1, err] = audioEngine->createSoundEffect(audioClip, isLooped); err != nullptr) {
            return errors::wrap(std::move(err), "failed to create sound effect");
        }
        else {
            soundEffect1_ = std::move(soundEffect1);
        }
        soundEffect1_->setVolume(1.0f);
    }

    // NOTE: Load .ogg audio file.
    if (auto [audioClip, clipErr] = loadAudioClip(fs_, gameHost_->getAudioEngine(), "/assets/sounds/synth.ogg"); clipErr != nullptr) {
        return errors::wrap(std::move(clipErr), "failed to load audio");
    }
    else {
        constexpr bool isLooped = true;
        if (auto [soundEffect2, err] = audioEngine->createSoundEffect(audioClip, isLooped); err != nullptr) {
            return errors::wrap(std::move(err), "failed to create sound effect");
        }
        else {
            soundEffect2_ = std::move(soundEffect2);
        }
        soundEffect2_->setVolume(1.0f);
    }

    // NOTE: Set main audio volume.
    audioEngine->setMainVolume(0.3f);

    auto mouse = gameHost_->getMouse();
    connect_(mouse->ButtonDown, [this](MouseButtons mouseButton) {
        if (mouseButton != MouseButtons::Left) {
            return;
        }

        const auto window = gameHost_->getWindow();
        const auto mouse = gameHost_->getMouse();
        const auto mouseState = mouse->getState();
        const auto clientBounds = window->getClientBounds();

        auto pos = mouseState.position;
        pos.x = pos.x - (clientBounds.width / 2);
        pos.y = -pos.y + (clientBounds.height / 2);

        if (pos.y > (clientBounds.height / 2) - 50) {
            return;
        }

        if (pos.x > 0) {
            soundEffect1_->stop();
            soundEffect1_->play();
        }
        else {
            if (soundEffect2_->getState() != SoundState::Playing) {
                soundEffect2_->play();
            }
            else {
                soundEffect2_->pause();
            }
        }
    });

    return nullptr;
}

void AudioClipTest::update()
{
}

void AudioClipTest::draw()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<float>(presentationParameters.backBufferWidth),
        static_cast<float>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

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

    const auto width = static_cast<float>(viewport.width);
    spriteBatch_->reset();
    spriteBatch_->setTransform(projectionMatrix);
    if (soundEffect2_->getState() != SoundState::Playing) {
        spriteFont_->draw(*spriteBatch_, "Click here to play BGM", Vector2{-width * 0.5f + 10.0f, 20.0f}, Color::createWhite(), 0.0f, Vector2{0.0f, 0.5f}, 1.0f);
    }
    else {
        spriteFont_->draw(*spriteBatch_, "Click here to pause BGM", Vector2{-width * 0.5f + 10.0f, 20.0f}, Color::createLime(), 0.0f, Vector2{0.0f, 0.5f}, 1.0f);
    }

    spriteFont_->draw(*spriteBatch_, "Click here to play SE", Vector2{width * 0.5f - 10.0f, -20.0f}, Color::createWhite(), 0.0f, Vector2{1.0f, 0.5f}, 1.0f);
    spriteBatch_->flush(commandList_, spritePipeline_);
    spriteBatch_->submit(graphicsDevice_);

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
