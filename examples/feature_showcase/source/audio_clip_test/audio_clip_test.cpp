#include "audio_clip_test.h"

namespace feature_showcase {

AudioClipTest::AudioClipTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error> AudioClipTest::initialize()
{
    auto assets = gameHost->getAssetManager();
    auto clock = gameHost->getClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->CreateCommandList();
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }

    spriteBatch = std::make_shared<SpriteBatch>(graphicsDevice, *assets);

    auto [font, fontErr] = assets->Load<TrueTypeFont>("Fonts/NotoSans/NotoSans-Regular.ttf");
    if (fontErr != nullptr) {
        return errors::wrap(std::move(fontErr), "failed to load a font file");
    }

    spriteFont = std::make_shared<SpriteFont>(graphicsDevice, font, 24.0f, 24.0f);
    spriteFont->PrepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");

    auto audioEngine = gameHost->getAudioEngine();

    // NOTE: Load .wav audio file.
    if (auto [audioClip, clipErr] = assets->Load<AudioClip>("Sounds/pong1.wav"); clipErr != nullptr) {
        return errors::wrap(std::move(clipErr), "failed to load audio");
    }
    else {
        constexpr bool isLooped = false;
        std::tie(soundEffect1, err) = audioEngine->CreateSoundEffect(audioClip, isLooped);
        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create sound effect");
        }
        soundEffect1->SetVolume(1.0f);
    }

    // NOTE: Load .ogg audio file.
    if (auto [audioClip, clipErr] = assets->Load<AudioClip>("Sounds/synth.ogg"); clipErr != nullptr) {
        return errors::wrap(std::move(clipErr), "failed to load audio");
    }
    else {
        constexpr bool isLooped = true;
        std::tie(soundEffect2, err) = audioEngine->CreateSoundEffect(audioClip, isLooped);
        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create sound effect");
        }
        soundEffect2->SetVolume(1.0f);
    }

    // NOTE: Set main audio volume.
    audioEngine->SetMainVolume(0.3f);

    auto mouse = gameHost->getMouse();
    connect(mouse->ButtonDown, [this](MouseButtons mouseButton) {
        if (mouseButton != MouseButtons::Left) {
            return;
        }

        const auto window = gameHost->getWindow();
        const auto mouse = gameHost->getMouse();
        const auto mouseState = mouse->GetState();
        const auto clientBounds = window->getClientBounds();

        auto pos = mouseState.Position;
        pos.x = pos.x - (clientBounds.width / 2);
        pos.y = -pos.y + (clientBounds.height / 2);

        if (pos.y > (clientBounds.height / 2) - 50) {
            return;
        }

        if (pos.x > 0) {
            soundEffect1->Stop();
            soundEffect1->Play();
        }
        else {
            if (soundEffect2->GetState() != SoundState::Playing) {
                soundEffect2->Play();
            }
            else {
                soundEffect2->Pause();
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
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

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

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    const auto width = static_cast<float>(viewport.width);
    spriteBatch->Begin(commandList, projectionMatrix);
    if (soundEffect2->GetState() != SoundState::Playing) {
        spriteFont->Draw(*spriteBatch, "Click here to play BGM", Vector2{-width * 0.5f + 10.0f, 20.0f}, Color::createWhite(), 0.0f, Vector2{0.0f, 0.5f}, 1.0f);
    }
    else {
        spriteFont->Draw(*spriteBatch, "Click here to pause BGM", Vector2{-width * 0.5f + 10.0f, 20.0f}, Color::createGreen(), 0.0f, Vector2{0.0f, 0.5f}, 1.0f);
    }

    spriteFont->Draw(*spriteBatch, "Click here to play SE", Vector2{width * 0.5f - 10.0f, -20.0f}, Color::createWhite(), 0.0f, Vector2{1.0f, 0.5f}, 1.0f);
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
