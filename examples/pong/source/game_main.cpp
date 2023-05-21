#include "game_main.h"
#include "pomdog/experimental/graphics/truetype_font_loader.h"
#include "pomdog/experimental/image_effects/chromatic_aberration.h"
#include "pomdog/experimental/image_effects/fxaa.h"
#include "pomdog/experimental/image_effects/fish_eye_effect.h"
#include "pomdog/experimental/image_effects/retro_crt_effect.h"
#include "pomdog/experimental/image_effects/vignette_effect.h"
#include <cmath>
#include <random>
#include <utility>

namespace pong {

GameMain::GameMain(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , window(gameHostIn->GetWindow())
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetCommandQueue())
    , assets(gameHostIn->GetAssetManager())
    , clock(gameHostIn->GetClock())
    , audioEngine(gameHostIn->GetAudioEngine())
    , postProcessCompositor(gameHostIn->GetGraphicsDevice())
    , textTimer(clock)
{
    window->SetAllowUserResizing(true);
}

std::unique_ptr<Error> GameMain::Initialize()
{
    // NOTE: Set window name
    window->SetTitle("Pomdog Pong");

    // NOTE: Set main volume
    audioEngine->SetMainVolume(0.4f);

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->CreateCommandList();
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to create graphics command list");
    }

    // NOTE: Create batch renderers
    primitiveBatch = std::make_shared<PrimitiveBatch>(graphicsDevice, *assets);
    spriteBatch = std::make_shared<SpriteBatch>(graphicsDevice, *assets);

    // NOTE: Prepare sprite font
    auto [font, fontErr] = assets->Load<TrueTypeFont>("fonts/NotoSans/NotoSans-BoldItalic.ttf");
    if (fontErr != nullptr) {
        return errors::Wrap(std::move(fontErr), "failed to load a font file");
    }
    spriteFont = std::make_shared<SpriteFont>(graphicsDevice, font, 26.0f, 26.0f);
    spriteFont->PrepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");
    spriteFont->SetDefaultCharacter(U'?');

    // NOTE: Load sound effects
    if (auto [audioClip, loadErr] = assets->Load<AudioClip>("sounds/pong1.wav"); loadErr != nullptr) {
        return errors::Wrap(std::move(loadErr), "failed to load audio clip");
    }
    else {
        std::tie(soundEffect1, err) = audioEngine->CreateSoundEffect(audioClip, false);
        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create sound effect");
        }
    }

    if (auto [audioClip, loadErr] = assets->Load<AudioClip>("sounds/pong2.wav"); loadErr != nullptr) {
        return errors::Wrap(std::move(loadErr), "failed to load audio clip");
    }
    else {
        std::tie(soundEffect2, err) = audioEngine->CreateSoundEffect(audioClip, false);
        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create sound effect");
        }
    }

    if (auto [audioClip, loadErr] = assets->Load<AudioClip>("sounds/pong3.wav"); loadErr != nullptr) {
        return errors::Wrap(std::move(loadErr), "failed to load audio clip");
    }
    else {
        std::tie(soundEffect3, err) = audioEngine->CreateSoundEffect(audioClip, false);
        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create sound effect");
        }
    }

    {
        auto fxaa = std::make_shared<FXAA>(graphicsDevice, *assets);
        auto fishEyeEffect = std::make_shared<FishEyeEffect>(graphicsDevice, *assets);
        auto vignetteEffect = std::make_shared<VignetteEffect>(graphicsDevice, *assets);
        auto chromaticAberration = std::make_shared<ChromaticAberration>(graphicsDevice, *assets);
        auto retroCrtEffect = std::make_shared<RetroCrtEffect>(graphicsDevice, *assets);

        vignetteEffect->SetIntensity(1.0f);
        fishEyeEffect->SetStrength(0.2f);

        auto presentationParameters = graphicsDevice->GetPresentationParameters();

        // NOTE: Create render target
        std::tie(renderTarget, err) = graphicsDevice->CreateRenderTarget2D(
            presentationParameters.BackBufferWidth,
            presentationParameters.BackBufferHeight,
            false,
            presentationParameters.BackBufferFormat);

        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create render target");
        }

        // NOTE: Create depth stencil buffer
        std::tie(depthStencilBuffer, err) = graphicsDevice->CreateDepthStencilBuffer(
            presentationParameters.BackBufferWidth,
            presentationParameters.BackBufferHeight,
            presentationParameters.DepthStencilFormat);

        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create render target");
        }

        postProcessCompositor.SetViewportSize(
            *graphicsDevice, presentationParameters.BackBufferWidth,
            presentationParameters.BackBufferHeight,
            presentationParameters.DepthStencilFormat);

        postProcessCompositor.Composite({
            fxaa,
            retroCrtEffect,
            chromaticAberration,
            fishEyeEffect,
            vignetteEffect,
            chromaticAberration,
        });

        connect(window->ClientSizeChanged, [this](int width, int height) {
            auto presentationParameters = graphicsDevice->GetPresentationParameters();

            renderTarget = std::get<0>(graphicsDevice->CreateRenderTarget2D(
                width,
                height,
                false,
                presentationParameters.BackBufferFormat));

            depthStencilBuffer = std::get<0>(graphicsDevice->CreateDepthStencilBuffer(
                width,
                height,
                presentationParameters.DepthStencilFormat));

            postProcessCompositor.SetViewportSize(
                *graphicsDevice, width, height,
                presentationParameters.DepthStencilFormat);
        });
    }
    {
        gameFieldSize = Rectangle{0, 0, 380, 280};
        gameFieldSize.x = -gameFieldSize.width / 2;
        gameFieldSize.y = -gameFieldSize.height / 2;

        player1.SetScore(0);
        player2.SetScore(0);

        auto keyboard = gameHost->GetKeyboard();
        input1.Reset(Keys::UpArrow, Keys::DownArrow, keyboard);
        input2.Reset(Keys::W, Keys::S, keyboard);

        connect(input1.Up, [this] {
            paddle1.positionOld = paddle1.position;
            auto position = paddle1.position;
            position.y += paddle1.speed * static_cast<float>(clock->GetFrameDuration().count());

            if (position.y > (gameFieldSize.height / 2 - paddle1.height / 2)) {
                position.y = (gameFieldSize.height / 2 - paddle1.height / 2);
            }
            paddle1.position = position;
        });

        connect(input1.Down, [this] {
            paddle1.positionOld = paddle1.position;
            auto position = paddle1.position;
            position.y -= paddle1.speed * clock->GetFrameDuration().count();

            if (position.y < -(gameFieldSize.height / 2 - paddle1.height / 2)) {
                position.y = -(gameFieldSize.height / 2 - paddle1.height / 2);
            }
            paddle1.position = position;
        });

        connect(input2.Up, [this] {
            paddle2.positionOld = paddle2.position;
            auto position = paddle2.position;
            position.y += paddle2.speed * clock->GetFrameDuration().count();

            if (position.y > (gameFieldSize.height / 2 - paddle2.height / 2)) {
                position.y = (gameFieldSize.height / 2 - paddle2.height / 2);
            }
            paddle2.position = position;
        });

        connect(input2.Down, [this] {
            paddle2.positionOld = paddle2.position;
            auto position = paddle2.position;
            position.y -= paddle2.speed * clock->GetFrameDuration().count();

            if (position.y < -(gameFieldSize.height / 2 - paddle2.height / 2)) {
                position.y = -(gameFieldSize.height / 2 - paddle2.height / 2);
            }
            paddle2.position = position;
        });
    }
    {
        // Header text
        textTimer.SetInterval(std::chrono::milliseconds(500));
        connect(textTimer.Elapsed, [this] {
            headerText = StringHelper::Format(
                "%.0lf sec\n%.0f fps   SCORE %d - %d",
                clock->GetTotalGameTime().count(),
                std::round(clock->GetFrameRate()),
                player1.GetScore(),
                player2.GetScore());
        });

        paddle1.position = Vector2{gameFieldSize.width / 2.0f, 0.0f};
        paddle2.position = Vector2{-gameFieldSize.width / 2.0f, 0.0f};

        pongScene = PongScenes::StartWaiting;
    }

    return nullptr;
}

void GameMain::Update()
{
    switch (pongScene) {
    case PongScenes::StartWaiting: {
        ball.position = Vector2::Zero();
        ball.positionOld = Vector2::Zero();
        ball.velocity = Vector2::Zero();

        auto keyboard = gameHost->GetKeyboard();
        startButtonConn = keyboard->KeyDown.Connect([this](Keys key) {
            if (key == Keys::Space) {
                pongScene = PongScenes::Prepare;
            }
        });
        scoreTextVisible = true;
        pongScene = PongScenes::Waiting;
        break;
    }
    case PongScenes::Waiting: {
        input1.Emit();
        input2.Emit();
        break;
    }
    case PongScenes::Prepare: {
        scoreTextVisible = false;
        startButtonConn.Disconnect();

        const float speed = 420.0f;
        std::mt19937 random(std::random_device{}());
        std::uniform_real_distribution<float> distribution(0.7f, 1.0f);
        static bool flipflop = false;
        flipflop = !flipflop;
        ball.velocity = math::Normalize(Vector2{(flipflop ? -1.0f : 1.0f), distribution(random)}) * speed;

        soundEffect2->Stop();
        soundEffect2->Play();

        pongScene = PongScenes::Playing;
        break;
    }
    case PongScenes::Playing: {
        input1.Emit();
        input2.Emit();

        ball.positionOld = ball.position;
        auto position = ball.position + (ball.velocity * clock->GetFrameDuration().count());
        ball.position = position;

        bool collision = false;

        if (position.x >= 0) {
            auto paddleCollider = paddle1.GetCollider();
            if (paddleCollider.Intersects(ball.GetCollider()) && ball.positionOld.x <= paddleCollider.min.x) {
                ball.velocity.x *= -1;
                collision = true;
            }
        }
        else {
            auto paddleCollider = paddle2.GetCollider();
            if (paddleCollider.Intersects(ball.GetCollider()) && ball.positionOld.x >= paddleCollider.max.x) {
                ball.velocity.x *= -1;
                collision = true;
            }
        }

        const auto halfWidth = gameFieldSize.width / 2;
        const auto halfHeight = gameFieldSize.height / 2;

        if (position.y >= halfHeight || position.y <= -halfHeight) {
            ball.velocity.y *= -1;
            position.y = ball.positionOld.y;
            ball.position = position;
            collision = true;
        }

        const float offset = 70.0f;
        if (position.x >= (halfWidth + offset)) {
            player1.SetScore(player1.GetScore() + 1);
            soundEffect3->Stop();
            soundEffect3->Play();
            pongScene = PongScenes::StartWaiting;
        }
        else if (position.x <= -(halfWidth + offset)) {
            player2.SetScore(player2.GetScore() + 1);
            soundEffect3->Stop();
            soundEffect3->Play();
            pongScene = PongScenes::StartWaiting;
        }

        if (collision) {
            soundEffect1->Stop();
            soundEffect1->Play();
        }
        break;
    }
    default:
        break;
    }
}

void GameMain::Draw()
{
    const auto backgroundColor = Color{32, 31, 30, 255};

    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    gpu::RenderPass pass;
    pass.RenderTargets[0] = {renderTarget, backgroundColor.ToVector4()};
    pass.DepthStencilBuffer = depthStencilBuffer;
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = std::uint8_t(0);
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    const auto projectionMatrix = Matrix4x4::CreatePerspectiveFieldOfViewLH(
        math::ToRadians(45.0f),
        viewport.GetAspectRatio(),
        0.01f,
        1000.0f);

    const auto cameraPosition = Vector3{0, 0, -512.0f};
    const auto cameraRotation = math::ToRadians(-15.0f);
    const auto viewMatrix = Matrix4x4::CreateRotationX(-cameraRotation) * Matrix4x4::CreateTranslation(-cameraPosition);
    const auto viewProjection = viewMatrix * projectionMatrix;

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    // NOTE: Draw primitives
    primitiveBatch->Begin(commandList, viewProjection);
    {
        // NOTE: Draw background
        {
            // Rectangle outline
            auto p1 = Vector2(static_cast<float>(gameFieldSize.GetLeft()), static_cast<float>(gameFieldSize.GetBottom()));
            auto p2 = Vector2(static_cast<float>(gameFieldSize.GetLeft()), static_cast<float>(gameFieldSize.GetTop()));
            auto p3 = Vector2(static_cast<float>(gameFieldSize.GetRight()), static_cast<float>(gameFieldSize.GetTop()));
            auto p4 = Vector2(static_cast<float>(gameFieldSize.GetRight()), static_cast<float>(gameFieldSize.GetBottom()));
            primitiveBatch->DrawLine(p1, p2, Color::White(), 2.0f);
            primitiveBatch->DrawLine(p2, p3, Color::White(), 2.0f);
            primitiveBatch->DrawLine(p3, p4, Color::White(), 2.0f);
            primitiveBatch->DrawLine(p4, p1, Color::White(), 2.0f);
        }
        {
            // Dotted line
            int count = 32;
            float offset = 0.5f;
            float startY = -gameFieldSize.height * 0.5f;
            float height = (gameFieldSize.height + (gameFieldSize.height / count * offset));
            for (int i = 0; i < count; ++i) {
                Vector2 start = {0.0f, height / count * i + startY};
                Vector2 end = {0.0f, height / count * (i + offset) + startY};
                primitiveBatch->DrawLine(start, end, Color::White(), 2.0f);
            }
        }

        // Paddle 1
        primitiveBatch->DrawRectangle(paddle1.position, 10.0f, paddle1.height, {0.5f, 0.5f}, Color::White());

        // Paddle 2
        primitiveBatch->DrawRectangle(paddle2.position, 10.0f, paddle2.height, {0.5f, 0.5f}, Color::White());

        // Ball
        primitiveBatch->DrawCircle(ball.position, 6.0f, 32, Color::White());
    }
    primitiveBatch->End();

    // NOTE: Draw sprites and fonts
    spriteBatch->Begin(commandList, Matrix4x4::CreateScale(0.002f) * viewProjection);
    spriteFont->Draw(*spriteBatch, "", Vector2::Zero(), Color::White(), 0.0f, Vector2{0.0f, 0.0f}, 1.0f);
    {
        // Header Text
        spriteFont->Draw(
            *spriteBatch,
            headerText,
            Vector2{-110, 140}, Color::White(), 0.0f, Vector2{0.0f, 0.0f}, 1.0f);

        // Footer Text
        spriteFont->Draw(
            *spriteBatch,
            "[SPACE] to start, WS and Up/Down to move",
            Vector2{-180, -170}, Color::White(), 0.0f, Vector2{0.0f, 0.0f}, 1.0f);
    }
    if (scoreTextVisible) {
        // "Player 1" Text
        spriteFont->Draw(
            *spriteBatch,
            "Player 1",
            Vector2{-135, 90}, Color::Yellow(), 0.0f, Vector2{0.0f, 0.0f}, 1.0f);

        // "Player 2" Text
        spriteFont->Draw(
            *spriteBatch,
            "Player 2",
            Vector2{65, 90}, Color::Yellow(), 0.0f, Vector2{0.0f, 0.0f}, 1.0f);

        // "Player 1" Score Text
        spriteFont->Draw(
            *spriteBatch,
            std::to_string(player1.GetScore()),
            Vector2{-110, 50}, Color::White(), 0.0f, Vector2{0.0f, 0.0f}, 2.0f);

        // "Player 2" Score Text
        spriteFont->Draw(
            *spriteBatch,
            std::to_string(player2.GetScore()),
            Vector2{80, 50}, Color::White(), 0.0f, Vector2{0.0f, 0.0f}, 2.0f);
    }
    spriteBatch->End();

    postProcessCompositor.Draw(*commandList, renderTarget);

    commandList->Close();

    // NOTE: Execute graphics commands
    commandQueue->Reset();
    commandQueue->PushbackCommandList(commandList);
    commandQueue->ExecuteCommandLists();
    commandQueue->Present();
}

} // namespace pong
