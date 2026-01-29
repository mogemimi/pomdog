#include "game_main.h"
#include "pomdog/experimental/graphics/truetype_font_loader.h"
#include "pomdog/experimental/image_effects/chromatic_aberration.h"
#include "pomdog/experimental/image_effects/fish_eye_effect.h"
#include "pomdog/experimental/image_effects/fxaa.h"
#include "pomdog/experimental/image_effects/retro_crt_effect.h"
#include "pomdog/experimental/image_effects/vignette_effect.h"
#include <cmath>
#include <random>
#include <utility>

namespace pong {

GameMain::GameMain(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , window(gameHostIn->getWindow())
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
    , assets(gameHostIn->getAssetManager())
    , clock(gameHostIn->getClock())
    , audioEngine(gameHostIn->getAudioEngine())
    , postProcessCompositor(gameHostIn->getGraphicsDevice())
    , textTimer(clock)
{
    window->setAllowUserResizing(true);
}

std::unique_ptr<Error> GameMain::initialize()
{
    // NOTE: Set window name
    window->setTitle("Pomdog Pong");

    // NOTE: Set main volume
    audioEngine->setMainVolume(0.4f);

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->createCommandList();
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }

    // NOTE: Create batch renderers
    primitiveBatch = std::make_shared<PrimitiveBatch>(graphicsDevice, *assets);
    spriteBatch = std::make_shared<SpriteBatch>(graphicsDevice, *assets);

    // NOTE: Prepare sprite font
    auto [font, fontErr] = assets->load<TrueTypeFont>("fonts/NotoSans/NotoSans-BoldItalic.ttf");
    if (fontErr != nullptr) {
        return errors::wrap(std::move(fontErr), "failed to load a font file");
    }
    spriteFont = std::make_shared<SpriteFont>(graphicsDevice, font, 26.0f, 26.0f);
    spriteFont->prepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");
    spriteFont->setDefaultCharacter(U'?');

    // NOTE: Load sound effects
    if (auto [audioClip, loadErr] = assets->load<AudioClip>("sounds/pong1.wav"); loadErr != nullptr) {
        return errors::wrap(std::move(loadErr), "failed to load audio clip");
    }
    else {
        std::tie(soundEffect1, err) = audioEngine->createSoundEffect(audioClip, false);
        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create sound effect");
        }
    }

    if (auto [audioClip, loadErr] = assets->load<AudioClip>("sounds/pong2.wav"); loadErr != nullptr) {
        return errors::wrap(std::move(loadErr), "failed to load audio clip");
    }
    else {
        std::tie(soundEffect2, err) = audioEngine->createSoundEffect(audioClip, false);
        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create sound effect");
        }
    }

    if (auto [audioClip, loadErr] = assets->load<AudioClip>("sounds/pong3.wav"); loadErr != nullptr) {
        return errors::wrap(std::move(loadErr), "failed to load audio clip");
    }
    else {
        std::tie(soundEffect3, err) = audioEngine->createSoundEffect(audioClip, false);
        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create sound effect");
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

        auto presentationParameters = graphicsDevice->getPresentationParameters();

        // NOTE: Create render target
        std::tie(renderTarget, err) = graphicsDevice->createRenderTarget2D(
            presentationParameters.backBufferWidth,
            presentationParameters.backBufferHeight,
            false,
            presentationParameters.backBufferFormat);

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create render target");
        }

        // NOTE: Create depth stencil buffer
        std::tie(depthStencilBuffer, err) = graphicsDevice->createDepthStencilBuffer(
            presentationParameters.backBufferWidth,
            presentationParameters.backBufferHeight,
            presentationParameters.depthStencilFormat);

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create render target");
        }

        postProcessCompositor.SetViewportSize(
            *graphicsDevice, presentationParameters.backBufferWidth,
            presentationParameters.backBufferHeight,
            presentationParameters.depthStencilFormat);

        postProcessCompositor.Composite({
            fxaa,
            retroCrtEffect,
            chromaticAberration,
            fishEyeEffect,
            vignetteEffect,
            chromaticAberration,
        });

        connect(window->clientSizeChanged, [this](int width, int height) {
            auto presentationParameters = graphicsDevice->getPresentationParameters();

            renderTarget = std::get<0>(graphicsDevice->createRenderTarget2D(
                width,
                height,
                false,
                presentationParameters.backBufferFormat));

            depthStencilBuffer = std::get<0>(graphicsDevice->createDepthStencilBuffer(
                width,
                height,
                presentationParameters.depthStencilFormat));

            postProcessCompositor.SetViewportSize(
                *graphicsDevice, width, height,
                presentationParameters.depthStencilFormat);
        });
    }
    {
        gameFieldSize = Rectangle{0, 0, 380, 280};
        gameFieldSize.x = -gameFieldSize.width / 2;
        gameFieldSize.y = -gameFieldSize.height / 2;

        player1.SetScore(0);
        player2.SetScore(0);

        auto keyboard = gameHost->getKeyboard();
        input1.Reset(Keys::UpArrow, Keys::DownArrow, keyboard);
        input2.Reset(Keys::W, Keys::S, keyboard);

        connect(input1.Up, [this] {
            paddle1.positionOld = paddle1.position;
            auto position = paddle1.position;
            position.y += paddle1.speed * static_cast<pomdog::f32>(clock->getFrameDuration().count());

            if (position.y > (gameFieldSize.height / 2 - paddle1.height / 2)) {
                position.y = (gameFieldSize.height / 2 - paddle1.height / 2);
            }
            paddle1.position = position;
        });

        connect(input1.Down, [this] {
            paddle1.positionOld = paddle1.position;
            auto position = paddle1.position;
            position.y -= paddle1.speed * static_cast<pomdog::f32>(clock->getFrameDuration().count());

            if (position.y < -(gameFieldSize.height / 2 - paddle1.height / 2)) {
                position.y = -(gameFieldSize.height / 2 - paddle1.height / 2);
            }
            paddle1.position = position;
        });

        connect(input2.Up, [this] {
            paddle2.positionOld = paddle2.position;
            auto position = paddle2.position;
            position.y += paddle2.speed * static_cast<pomdog::f32>(clock->getFrameDuration().count());

            if (position.y > (gameFieldSize.height / 2 - paddle2.height / 2)) {
                position.y = (gameFieldSize.height / 2 - paddle2.height / 2);
            }
            paddle2.position = position;
        });

        connect(input2.Down, [this] {
            paddle2.positionOld = paddle2.position;
            auto position = paddle2.position;
            position.y -= paddle2.speed * static_cast<pomdog::f32>(clock->getFrameDuration().count());

            if (position.y < -(gameFieldSize.height / 2 - paddle2.height / 2)) {
                position.y = -(gameFieldSize.height / 2 - paddle2.height / 2);
            }
            paddle2.position = position;
        });
    }
    {
        // Header text
        textTimer.setInterval(std::chrono::milliseconds(500));
        connect(textTimer.elapsed, [this] {
            headerText = strings::format(
                "%.0lf sec\n%.0f fps   SCORE %d - %d",
                clock->getTotalGameTime().count(),
                std::round(clock->getFrameRate()),
                player1.GetScore(),
                player2.GetScore());
        });

        paddle1.position = Vector2{gameFieldSize.width / 2.0f, 0.0f};
        paddle2.position = Vector2{-gameFieldSize.width / 2.0f, 0.0f};

        pongScene = PongScenes::StartWaiting;
    }

    return nullptr;
}

void GameMain::update()
{
    switch (pongScene) {
    case PongScenes::StartWaiting: {
        ball.position = Vector2::createZero();
        ball.positionOld = Vector2::createZero();
        ball.velocity = Vector2::createZero();

        auto keyboard = gameHost->getKeyboard();
        startButtonConn = keyboard->KeyDown.connect([this](Keys key) {
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
        startButtonConn.disconnect();

        const float speed = 420.0f;
        std::mt19937 random(std::random_device{}());
        std::uniform_real_distribution<float> distribution(0.7f, 1.0f);
        static bool flipflop = false;
        flipflop = !flipflop;
        ball.velocity = math::normalize(Vector2{(flipflop ? -1.0f : 1.0f), distribution(random)}) * speed;

        soundEffect2->stop();
        soundEffect2->play();

        pongScene = PongScenes::Playing;
        break;
    }
    case PongScenes::Playing: {
        input1.Emit();
        input2.Emit();

        ball.positionOld = ball.position;
        auto position = ball.position + (ball.velocity * static_cast<pomdog::f32>(clock->getFrameDuration().count()));
        ball.position = position;

        bool collision = false;

        if (position.x >= 0) {
            auto paddleCollider = paddle1.GetCollider();
            if (paddleCollider.intersects(ball.GetCollider()) && ball.positionOld.x <= paddleCollider.min.x) {
                ball.velocity.x *= -1;
                collision = true;
            }
        }
        else {
            auto paddleCollider = paddle2.GetCollider();
            if (paddleCollider.intersects(ball.GetCollider()) && ball.positionOld.x >= paddleCollider.max.x) {
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
            soundEffect3->stop();
            soundEffect3->play();
            pongScene = PongScenes::StartWaiting;
        }
        else if (position.x <= -(halfWidth + offset)) {
            player2.SetScore(player2.GetScore() + 1);
            soundEffect3->stop();
            soundEffect3->play();
            pongScene = PongScenes::StartWaiting;
        }

        if (collision) {
            soundEffect1->stop();
            soundEffect1->play();
        }
        break;
    }
    default:
        break;
    }
}

void GameMain::draw()
{
    const auto backgroundColor = Color{32, 31, 30, 255};

    auto presentationParameters = graphicsDevice->getPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {renderTarget, backgroundColor.toVector4()};
    pass.depthStencilBuffer = depthStencilBuffer;
    pass.clearDepth = 1.0f;
    pass.clearStencil = std::uint8_t(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    const auto projectionMatrix = Matrix4x4::createPerspectiveFieldOfViewLH(
        math::toRadians(45.0f),
        viewport.getAspectRatio(),
        0.01f,
        1000.0f);

    const auto cameraPosition = Vector3{0, 0, -512.0f};
    const auto cameraRotation = math::toRadians(-15.0f);
    const auto viewMatrix = Matrix4x4::createRotationX(-cameraRotation) * Matrix4x4::createTranslation(-cameraPosition);
    const auto viewProjection = viewMatrix * projectionMatrix;

    commandList->reset();
    commandList->setRenderPass(std::move(pass));

    // NOTE: Draw primitives
    primitiveBatch->begin(commandList, viewProjection);
    {
        // NOTE: Draw background
        {
            // Rectangle outline
            auto p1 = Vector2(static_cast<float>(gameFieldSize.getLeft()), static_cast<float>(gameFieldSize.getBottom()));
            auto p2 = Vector2(static_cast<float>(gameFieldSize.getLeft()), static_cast<float>(gameFieldSize.getTop()));
            auto p3 = Vector2(static_cast<float>(gameFieldSize.getRight()), static_cast<float>(gameFieldSize.getTop()));
            auto p4 = Vector2(static_cast<float>(gameFieldSize.getRight()), static_cast<float>(gameFieldSize.getBottom()));
            primitiveBatch->drawLine(p1, p2, Color::createWhite(), 2.0f);
            primitiveBatch->drawLine(p2, p3, Color::createWhite(), 2.0f);
            primitiveBatch->drawLine(p3, p4, Color::createWhite(), 2.0f);
            primitiveBatch->drawLine(p4, p1, Color::createWhite(), 2.0f);
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
                primitiveBatch->drawLine(start, end, Color::createWhite(), 2.0f);
            }
        }

        // Paddle 1
        primitiveBatch->drawRectangle(paddle1.position, 10.0f, paddle1.height, {0.5f, 0.5f}, Color::createWhite());

        // Paddle 2
        primitiveBatch->drawRectangle(paddle2.position, 10.0f, paddle2.height, {0.5f, 0.5f}, Color::createWhite());

        // Ball
        primitiveBatch->drawCircle(ball.position, 6.0f, 32, Color::createWhite());
    }
    primitiveBatch->end();

    // NOTE: Draw sprites and fonts
    spriteBatch->begin(commandList, Matrix4x4::createScale(0.002f) * viewProjection);
    spriteFont->draw(*spriteBatch, "", Vector2::createZero(), Color::createWhite(), 0.0f, Vector2{0.0f, 0.0f}, 1.0f);
    {
        // Header Text
        spriteFont->draw(
            *spriteBatch,
            headerText,
            Vector2{-110, 140}, Color::createWhite(), 0.0f, Vector2{0.0f, 0.0f}, 1.0f);

        // Footer Text
        spriteFont->draw(
            *spriteBatch,
            "[SPACE] to start, WS and Up/Down to move",
            Vector2{-180, -170}, Color::createWhite(), 0.0f, Vector2{0.0f, 0.0f}, 1.0f);
    }
    if (scoreTextVisible) {
        // "Player 1" Text
        spriteFont->draw(
            *spriteBatch,
            "Player 1",
            Vector2{-135, 90}, Color::createYellow(), 0.0f, Vector2{0.0f, 0.0f}, 1.0f);

        // "Player 2" Text
        spriteFont->draw(
            *spriteBatch,
            "Player 2",
            Vector2{65, 90}, Color::createYellow(), 0.0f, Vector2{0.0f, 0.0f}, 1.0f);

        // "Player 1" Score Text
        spriteFont->draw(
            *spriteBatch,
            std::to_string(player1.GetScore()),
            Vector2{-110, 50}, Color::createWhite(), 0.0f, Vector2{0.0f, 0.0f}, 2.0f);

        // "Player 2" Score Text
        spriteFont->draw(
            *spriteBatch,
            std::to_string(player2.GetScore()),
            Vector2{80, 50}, Color::createWhite(), 0.0f, Vector2{0.0f, 0.0f}, 2.0f);
    }
    spriteBatch->end();

    postProcessCompositor.Draw(*commandList, renderTarget);

    commandList->close();

    // NOTE: Execute graphics commands
    commandQueue->reset();
    commandQueue->pushBackCommandList(commandList);
    commandQueue->executeCommandLists();
    commandQueue->present();
}

} // namespace pong
