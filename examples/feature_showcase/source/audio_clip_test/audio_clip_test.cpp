#include "audio_clip_test.h"

namespace feature_showcase {

AudioClipTest::AudioClipTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetCommandQueue())
{
}

std::unique_ptr<Error> AudioClipTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->CreateCommandList();
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to create graphics command list");
    }

    spriteBatch = std::make_shared<SpriteBatch>(graphicsDevice, *assets);

    auto [font, fontErr] = assets->Load<TrueTypeFont>("Fonts/NotoSans/NotoSans-Regular.ttf");
    if (fontErr != nullptr) {
        return errors::Wrap(std::move(fontErr), "failed to load a font file");
    }

    spriteFont = std::make_shared<SpriteFont>(graphicsDevice, font, 24.0f, 24.0f);
    spriteFont->PrepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");

    auto audioEngine = gameHost->GetAudioEngine();

    // NOTE: Load .wav audio file.
    if (auto [audioClip, clipErr] = assets->Load<AudioClip>("Sounds/pong1.wav"); clipErr != nullptr) {
        return errors::Wrap(std::move(clipErr), "failed to load audio");
    }
    else {
        constexpr bool isLooped = false;
        std::tie(soundEffect1, err) = audioEngine->CreateSoundEffect(audioClip, isLooped);
        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create sound effect");
        }
        soundEffect1->SetVolume(1.0f);
    }

    // NOTE: Load .ogg audio file.
    if (auto [audioClip, clipErr] = assets->Load<AudioClip>("Sounds/synth.ogg"); clipErr != nullptr) {
        return errors::Wrap(std::move(clipErr), "failed to load audio");
    }
    else {
        constexpr bool isLooped = true;
        std::tie(soundEffect2, err) = audioEngine->CreateSoundEffect(audioClip, isLooped);
        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create sound effect");
        }
        soundEffect2->SetVolume(1.0f);
    }

    // NOTE: Set main audio volume.
    audioEngine->SetMainVolume(0.3f);

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

        if (pos.Y > (window->GetClientBounds().Height / 2) - 50) {
            return;
        }

        if (pos.X > 0) {
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

void AudioClipTest::Update()
{
}

void AudioClipTest::Draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        static_cast<float>(presentationParameters.BackBufferWidth),
        static_cast<float>(presentationParameters.BackBufferHeight),
        0.0f,
        100.0f);

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

    const auto width = static_cast<float>(viewport.Width);
    spriteBatch->Begin(commandList, projectionMatrix);
    if (soundEffect2->GetState() != SoundState::Playing) {
        spriteFont->Draw(*spriteBatch, "Click here to play BGM", Vector2{-width * 0.5f + 10.0f, 20.0f}, Color::White(), 0.0f, Vector2{0.0f, 0.5f}, 1.0f);
    }
    else {
        spriteFont->Draw(*spriteBatch, "Click here to pause BGM", Vector2{-width * 0.5f + 10.0f, 20.0f}, Color::Green(), 0.0f, Vector2{0.0f, 0.5f}, 1.0f);
    }

    spriteFont->Draw(*spriteBatch, "Click here to play SE", Vector2{width * 0.5f - 10.0f, -20.0f}, Color::White(), 0.0f, Vector2{1.0f, 0.5f}, 1.0f);
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
