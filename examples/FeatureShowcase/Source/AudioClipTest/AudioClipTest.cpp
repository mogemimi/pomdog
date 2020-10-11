#include "AudioClipTest.hpp"

namespace FeatureShowcase {

AudioClipTest::AudioClipTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

void AudioClipTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();
    commandList = std::make_shared<GraphicsCommandList>(*graphicsDevice);
    spriteBatch = std::make_shared<SpriteBatch>(graphicsDevice, *assets);

    auto [font, fontErr] = assets->Load<TrueTypeFont>("Fonts/NotoSans/NotoSans-Regular.ttf");
    if (fontErr != nullptr) {
        Log::Critical("Error", "failed to load a font file: " + fontErr->ToString());
    }

    spriteFont = std::make_shared<SpriteFont>(graphicsDevice, font, 24.0f, 24.0f);
    spriteFont->PrepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");

    auto audioEngine = gameHost->GetAudioEngine();

    // NOTE: Load .wav audio file.
    if (auto [audioClip, err] = assets->Load<AudioClip>("Sounds/pong1.wav"); err != nullptr) {
        Log::Verbose("failed to load audio: " + err->ToString());
    }
    else {
        constexpr bool isLooped = false;
        std::tie(soundEffect1, err) = audioEngine->CreateSoundEffect(audioClip, isLooped);
        if (err != nullptr) {
            Log::Verbose("failed to create sound effect: " + err->ToString());
        }
        soundEffect1->SetVolume(1.0f);
    }

    // NOTE: Load .ogg audio file.
    if (auto [audioClip, err] = assets->Load<AudioClip>("Sounds/synth.ogg"); err != nullptr) {
        Log::Verbose("failed to load audio: " + err->ToString());
    }
    else {
        constexpr bool isLooped = true;
        std::tie(soundEffect2, err) = audioEngine->CreateSoundEffect(audioClip, isLooped);
        if (err != nullptr) {
            Log::Verbose("failed to create sound effect: " + err->ToString());
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

    Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    RenderPass pass;
    pass.RenderTargets[0] = {nullptr, Color::CornflowerBlue.ToVector4()};
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = 0;
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    const auto width = static_cast<float>(viewport.Width);
    spriteBatch->Begin(commandList, projectionMatrix);
    if (soundEffect2->GetState() != SoundState::Playing) {
        spriteFont->Draw(*spriteBatch, "Click here to play BGM", Vector2{-width * 0.5f + 10.0f, 20.0f}, Color::White, 0.0f, Vector2{0.0f, 0.5f}, 1.0f);
    }
    else {
        spriteFont->Draw(*spriteBatch, "Click here to pause BGM", Vector2{-width * 0.5f + 10.0f, 20.0f}, Color::Green, 0.0f, Vector2{0.0f, 0.5f}, 1.0f);
    }

    spriteFont->Draw(*spriteBatch, "Click here to play SE", Vector2{width * 0.5f - 10.0f, -20.0f}, Color::White, 0.0f, Vector2{1.0f, 0.5f}, 1.0f);
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
