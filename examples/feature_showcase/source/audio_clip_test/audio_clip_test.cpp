#include "audio_clip_test.h"

namespace feature_showcase {

AudioClipTest::AudioClipTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost(gameHostIn)
    , fs_(fs)
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
AudioClipTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/, int /*argc*/, const char* const* /*argv*/)
{
    auto clock = gameHost->getClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->createCommandList();
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }

    spriteBatch = std::make_shared<SpriteBatch>();
    if (auto spriteBatchErr = spriteBatch->initialize(fs_, graphicsDevice); spriteBatchErr != nullptr) {
        return errors::wrap(std::move(spriteBatchErr), "failed to initialize SpriteBatch");
    }

    auto [font, fontErr] = loadTrueTypeFont(fs_, "/assets/fonts/NotoSans-Regular.ttf");
    if (fontErr != nullptr) {
        return errors::wrap(std::move(fontErr), "failed to load a font file");
    }

    spriteFont = std::make_shared<SpriteFont>();
    if (auto spriteFontErr = spriteFont->initialize(graphicsDevice, font, 24.0f, 24.0f); spriteFontErr != nullptr) {
        return errors::wrap(std::move(spriteFontErr), "failed to initialize SpriteFont");
    }
    spriteFont->prepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");

    auto audioEngine = gameHost->getAudioEngine();

    // NOTE: Load .wav audio file.
    if (auto [audioClip, clipErr] = loadAudioClip(fs_, gameHost->getAudioEngine(), "/assets/sounds/pong1.wav"); clipErr != nullptr) {
        return errors::wrap(std::move(clipErr), "failed to load audio");
    }
    else {
        constexpr bool isLooped = false;
        std::tie(soundEffect1, err) = audioEngine->createSoundEffect(audioClip, isLooped);
        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create sound effect");
        }
        soundEffect1->setVolume(1.0f);
    }

    // NOTE: Load .ogg audio file.
    if (auto [audioClip, clipErr] = loadAudioClip(fs_, gameHost->getAudioEngine(), "/assets/sounds/synth.ogg"); clipErr != nullptr) {
        return errors::wrap(std::move(clipErr), "failed to load audio");
    }
    else {
        constexpr bool isLooped = true;
        std::tie(soundEffect2, err) = audioEngine->createSoundEffect(audioClip, isLooped);
        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create sound effect");
        }
        soundEffect2->setVolume(1.0f);
    }

    // NOTE: Set main audio volume.
    audioEngine->setMainVolume(0.3f);

    auto mouse = gameHost->getMouse();
    connect(mouse->ButtonDown, [this](MouseButtons mouseButton) {
        if (mouseButton != MouseButtons::Left) {
            return;
        }

        const auto window = gameHost->getWindow();
        const auto mouse = gameHost->getMouse();
        const auto mouseState = mouse->getState();
        const auto clientBounds = window->getClientBounds();

        auto pos = mouseState.position;
        pos.x = pos.x - (clientBounds.width / 2);
        pos.y = -pos.y + (clientBounds.height / 2);

        if (pos.y > (clientBounds.height / 2) - 50) {
            return;
        }

        if (pos.x > 0) {
            soundEffect1->stop();
            soundEffect1->play();
        }
        else {
            if (soundEffect2->getState() != SoundState::Playing) {
                soundEffect2->play();
            }
            else {
                soundEffect2->pause();
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
    auto presentationParameters = graphicsDevice->getPresentationParameters();

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

    commandList->reset();
    commandList->setRenderPass(std::move(pass));

    const auto width = static_cast<float>(viewport.width);
    spriteBatch->begin(commandList, projectionMatrix);
    if (soundEffect2->getState() != SoundState::Playing) {
        spriteFont->draw(*spriteBatch, "Click here to play BGM", Vector2{-width * 0.5f + 10.0f, 20.0f}, Color::createWhite(), 0.0f, Vector2{0.0f, 0.5f}, 1.0f);
    }
    else {
        spriteFont->draw(*spriteBatch, "Click here to pause BGM", Vector2{-width * 0.5f + 10.0f, 20.0f}, Color::createLime(), 0.0f, Vector2{0.0f, 0.5f}, 1.0f);
    }

    spriteFont->draw(*spriteBatch, "Click here to play SE", Vector2{width * 0.5f - 10.0f, -20.0f}, Color::createWhite(), 0.0f, Vector2{1.0f, 0.5f}, 1.0f);
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
