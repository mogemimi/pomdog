#include "PongGame.hpp"
#include <Pomdog.Experimental/ImageEffects/FXAA.hpp>
#include <Pomdog.Experimental/ImageEffects/VignetteEffect.hpp>
#include <Pomdog.Experimental/ImageEffects/FishEyeEffect.hpp>
#include <Pomdog.Experimental/ImageEffects/ChromaticAberration.hpp>
#include <Pomdog.Experimental/ImageEffects/RetroCrtEffect.hpp>
#include <Pomdog.Experimental/Gameplay2D/PrimitiveRenderable.hpp>
#include <Pomdog.Experimental/Gameplay2D/SpriteRenderable.hpp>
#include <Pomdog.Experimental/Gameplay2D/TextRenderable.hpp>
#include <Pomdog/Experimental/Graphics/PolygonBatch.hpp>
#include <Pomdog/Experimental/Graphics/SpriteBatch.hpp>
#include <Pomdog/Experimental/Graphics/TrueTypeFont.hpp>
#include <utility>
#include <cmath>
#include <random>

namespace Pong {

PongGame::PongGame(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , window(gameHostIn->GetWindow())
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
    , assets(gameHostIn->GetAssetManager())
    , clock(gameHostIn->GetClock())
    , audioEngine(gameHostIn->GetAudioEngine())
    , gameEngine(gameHostIn)
    , textTimer(clock)
    , sceneDirector(gameHostIn)
{
    Log::Info("Hello");
    window->SetAllowUserResizing(true);
}

void PongGame::Initialize()
{
    // Set window name
    window->SetTitle("Pomdog Pong");

    // Set master volume
    audioEngine->SetMasterVolume(0.4f);

    auto & entityManager = gameEngine.entityManager;

    {
        soundEffect1 = std::make_shared<SoundEffect>(*audioEngine,
            assets->Load<AudioClip>("sounds/pong1.wav"), false);

        soundEffect2 = std::make_shared<SoundEffect>(*audioEngine,
            assets->Load<AudioClip>("sounds/pong2.wav"), false);

        soundEffect3 = std::make_shared<SoundEffect>(*audioEngine,
            assets->Load<AudioClip>("sounds/pong3.wav"), false);
    }
    {
        gameFieldSize = Rectangle{0, 0, 380, 280};
        gameFieldSize.X = -gameFieldSize.Width / 2;
        gameFieldSize.Y = -gameFieldSize.Height / 2;

        player1.SetScore(0);
        player2.SetScore(0);

        auto keyboard = gameHost->GetKeyboard();
        input1.Reset(Keys::UpArrow, Keys::DownArrow, keyboard);
        input2.Reset(Keys::W, Keys::S, keyboard);

        connect(input1.Up, [this] {
            paddle1.PositionOld = paddle1.GetPosition();
            auto position = paddle1.GetPosition();
            position.Y += paddle1.Speed * clock->GetFrameDuration().count();

            if (position.Y > (gameFieldSize.Height / 2 - paddle1.Height / 2)) {
                position.Y = (gameFieldSize.Height / 2 - paddle1.Height / 2);
            }
            paddle1.SetPosition(position);
        });

        connect(input1.Down, [this] {
            paddle1.PositionOld = paddle1.GetPosition();
            auto position = paddle1.GetPosition();
            position.Y -= paddle1.Speed * clock->GetFrameDuration().count();

            if (position.Y < -(gameFieldSize.Height / 2 - paddle1.Height / 2)) {
                position.Y = -(gameFieldSize.Height / 2 - paddle1.Height / 2);
            }
            paddle1.SetPosition(position);
        });

        connect(input2.Up, [this] {
            paddle2.PositionOld = paddle2.GetPosition();
            auto position = paddle2.GetPosition();
            position.Y += paddle2.Speed * clock->GetFrameDuration().count();

            if (position.Y > (gameFieldSize.Height / 2 - paddle2.Height / 2)) {
                position.Y = (gameFieldSize.Height / 2 - paddle2.Height / 2);
            }
            paddle2.SetPosition(position);
        });
        
        connect(input2.Down, [this] {
            paddle2.PositionOld = paddle2.GetPosition();
            auto position = paddle2.GetPosition();
            position.Y -= paddle2.Speed * clock->GetFrameDuration().count();

            if (position.Y < -(gameFieldSize.Height / 2 - paddle2.Height / 2)) {
                position.Y = -(gameFieldSize.Height / 2 - paddle2.Height / 2);
            }
            paddle2.SetPosition(position);
        });
    }
    {
        auto font = std::make_shared<TrueTypeFont>(*assets, "fonts/NotoSans/NotoSans-BoldItalic.ttf");
        spriteFont = std::make_shared<SpriteFont>(graphicsDevice, font, 26.0f, 26.0f);
        spriteFont->PrepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");
        spriteFont->SetDefaultCharacter(U'?');
    }
    {
        auto fxaa = std::make_shared<FXAA>(graphicsDevice, *assets);
        auto fishEyeEffect = std::make_shared<FishEyeEffect>(graphicsDevice, *assets);
        auto vignetteEffect = std::make_shared<VignetteEffect>(graphicsDevice, *assets);
        auto chromaticAberration = std::make_shared<ChromaticAberration>(graphicsDevice, *assets);
        auto retroCrtEffect = std::make_shared<RetroCrtEffect>(graphicsDevice, *assets);

        vignetteEffect->SetIntensity(1.0f);
        fishEyeEffect->SetStrength(0.2f);

        gameEngine.Composite({
            fxaa,
            retroCrtEffect,
            chromaticAberration,
            fishEyeEffect,
            vignetteEffect,
            chromaticAberration,
        });
    }
    {
        // Camera
        auto entity = entityManager.CreateEntity({
            std::make_shared<ComponentCreator<Transform>>(),
            std::make_shared<ComponentCreator<CameraComponent>>()
        });

        auto camera = entity.GetComponent<CameraComponent>();
        camera->SetBackgroundColor(Color{32, 31, 30, 255});
        camera->SetNear(0.01f);
        camera->SetFar(1000.0f);
        camera->SetProjectionType(ProjectionType::Perspective);
        camera->SetFieldOfView(MathHelper::ToRadians<float>(45.0f));

        auto transform = entity.GetComponent<Transform>();
        transform->SetPosition(Vector3{0, -140, -512.0f});
        transform->SetRotationX(MathHelper::ToRadians<float>(-15.0f));

        gameEngine.SetCamera(entity);
    }
    {
        // Background
        auto entity = entityManager.CreateEntity({
            std::make_shared<ComponentCreator<Transform>>(),
            std::make_shared<ComponentCreator<PrimitiveRenderable>>(graphicsDevice)
        });

        auto graphicsComponent = entity.GetComponent<PrimitiveRenderable>();
        graphicsComponent->SetDrawOrder(1);
        {
            // Rectangle outline
            auto p1 = Vector2(gameFieldSize.GetLeft(), gameFieldSize.GetBottom());
            auto p2 = Vector2(gameFieldSize.GetLeft(), gameFieldSize.GetTop());
            auto p3 = Vector2(gameFieldSize.GetRight(), gameFieldSize.GetTop());
            auto p4 = Vector2(gameFieldSize.GetRight(), gameFieldSize.GetBottom());
            graphicsComponent->DrawLine(p1, p2, Color::White, 2.0f);
            graphicsComponent->DrawLine(p2, p3, Color::White, 2.0f);
            graphicsComponent->DrawLine(p3, p4, Color::White, 2.0f);
            graphicsComponent->DrawLine(p4, p1, Color::White, 2.0f);
        }
        {
            // Dotted line
            int count = 32;
            float offset = 0.5f;
            float startY = -gameFieldSize.Height * 0.5f;
            float height = (gameFieldSize.Height + (gameFieldSize.Height / count * offset));
            for (int i = 0; i < count; ++i) {
                Vector2 start = {0.0f, height / count * i + startY};
                Vector2 end = {0.0f, height / count * (i + offset) + startY};
                graphicsComponent->DrawLine(start, end, Color::White, 2.0f);
            }
        }
    }
    {
        // Ball
        auto entity = entityManager.CreateEntity({
            std::make_shared<ComponentCreator<Transform>>(),
            std::make_shared<ComponentCreator<PrimitiveRenderable>>(graphicsDevice)
        });

        auto graphicsComponent = entity.GetComponent<PrimitiveRenderable>();
        graphicsComponent->SetDrawOrder(2);
        graphicsComponent->DrawCircle(Vector2::Zero, 6.0f, 32, Color::White);

        ball.transform = entity.GetComponent<Transform>();
        ball.SetPosition(Vector2::Zero);
    }
    {
        // Paddle 1
        auto entity = entityManager.CreateEntity({
            std::make_shared<ComponentCreator<Transform>>(),
            std::make_shared<ComponentCreator<PrimitiveRenderable>>(graphicsDevice)
        });

        auto graphicsComponent = entity.GetComponent<PrimitiveRenderable>();
        graphicsComponent->SetDrawOrder(3);
        graphicsComponent->DrawRectangle(
            Vector2::Zero, 10.0f, paddle1.Height, {0.5f, 0.5f}, Color::White);

        paddle1.transform = entity.GetComponent<Transform>();
        paddle1.SetPosition({gameFieldSize.Width / 2.0f, 0.0f});
    }
    {
        // Paddle 2
        auto entity = entityManager.CreateEntity({
            std::make_shared<ComponentCreator<Transform>>(),
            std::make_shared<ComponentCreator<PrimitiveRenderable>>(graphicsDevice)
        });

        auto graphicsComponent = entity.GetComponent<PrimitiveRenderable>();
        graphicsComponent->SetDrawOrder(3);
        graphicsComponent->DrawRectangle(
            Vector2::Zero, 10.0f, paddle2.Height, {0.5f, 0.5f}, Color::White);

        paddle2.transform = entity.GetComponent<Transform>();
        paddle2.SetPosition({-gameFieldSize.Width / 2.0f, 0.0f});
    }
    {
        // Header Text
        auto entity = entityManager.CreateEntity({
            std::make_shared<ComponentCreator<Transform>>(),
            std::make_shared<ComponentCreator<TextRenderable>>()
        });

        auto transform = entity.GetComponent<Transform>();
        transform->SetPosition2D(Vector2{-110, 180});

        auto text = entity.GetComponent<TextRenderable>();
        text->SetDrawOrder(4);
        text->SetFont(spriteFont);
        text->SetTextColor(Color::White);

        textTimer.SetInterval(std::chrono::milliseconds(500));
        connect(textTimer.Elapsed, [this, text] {
            text->SetText(StringHelper::Format(
                "%.0lf sec\n%.0f fps   SCORE %d - %d",
                clock->GetTotalGameTime().count(),
                std::round(clock->GetFrameRate()),
                player1.GetScore(),
                player2.GetScore()));
        });
    }
    {
        // Footer Text
        auto entity = entityManager.CreateEntity({
            std::make_shared<ComponentCreator<Transform>>(),
            std::make_shared<ComponentCreator<TextRenderable>>()
        });

        auto transform = entity.GetComponent<Transform>();
        transform->SetPosition2D(Vector2{-180, -160});

        auto text = entity.GetComponent<TextRenderable>();
        text->SetDrawOrder(4);
        text->SetFont(spriteFont);
        text->SetText("[SPACE] to start, WS and Up/Down to move");
        text->SetTextColor(Color::White);
    }
    {
        // "Player 1" Text
        auto entity = entityManager.CreateEntity({
            std::make_shared<ComponentCreator<Transform>>(),
            std::make_shared<ComponentCreator<TextRenderable>>()
        });

        auto transform = entity.GetComponent<Transform>();
        transform->SetPosition2D(Vector2{-135, 90});

        auto text = entity.GetComponent<TextRenderable>();
        text->SetDrawOrder(4);
        text->SetFont(spriteFont);
        text->SetText("Player 1");
        text->SetTextColor(Color::Yellow);

        scoreTextLabels.push_back(entity);
    }
    {
        // "Player 2" Text
        auto entity = entityManager.CreateEntity({
            std::make_shared<ComponentCreator<Transform>>(),
            std::make_shared<ComponentCreator<TextRenderable>>()
        });

        auto transform = entity.GetComponent<Transform>();
        transform->SetPosition2D(Vector2{65, 90});

        auto text = entity.GetComponent<TextRenderable>();
        text->SetDrawOrder(4);
        text->SetFont(spriteFont);
        text->SetText("Player 2");
        text->SetTextColor(Color::Yellow);

        scoreTextLabels.push_back(entity);
    }
    {
        // "Player 1" Score Text
        auto entity = entityManager.CreateEntity({
            std::make_shared<ComponentCreator<Transform>>(),
            std::make_shared<ComponentCreator<TextRenderable>>()
        });

        auto transform = entity.GetComponent<Transform>();
        transform->SetPosition2D(Vector2{-110, 50});
        transform->SetScale(2.0f);

        auto text = entity.GetComponent<TextRenderable>();
        text->SetDrawOrder(4);
        text->SetFont(spriteFont);
        text->SetText(std::to_string(player1.GetScore()));
        text->SetTextColor(Color::White);

        connect(player1.ScoreChanged, [text](int score) {
            text->SetText(std::to_string(score));
        });

        scoreTextLabels.push_back(entity);
    }
    {
        // "Player 2" Score Text
        auto entity = entityManager.CreateEntity({
            std::make_shared<ComponentCreator<Transform>>(),
            std::make_shared<ComponentCreator<TextRenderable>>()
        });

        auto transform = entity.GetComponent<Transform>();
        transform->SetPosition2D(Vector2{80, 50});
        transform->SetScale(2.0f);

        auto text = entity.GetComponent<TextRenderable>();
        text->SetDrawOrder(4);
        text->SetFont(spriteFont);
        text->SetText(std::to_string(player2.GetScore()));
        text->SetTextColor(Color::White);

        connect(player2.ScoreChanged, [text](int score) {
            text->SetText(std::to_string(score));
        });

        scoreTextLabels.push_back(entity);
    }
    {
        auto scene = this->CreateNewGameScene();
        sceneDirector.Run(scene);
    }
}

std::shared_ptr<GameScene> PongGame::CreateNewGameScene()
{
    auto scene = std::make_shared<GameScene>();
    std::weak_ptr<GameScene> weakScene = scene;
    scene->OnEnter = [this, weakScene] {
        ball.SetPosition(Vector2::Zero);
        ball.PositionOld = Vector2::Zero;
        ball.Velocity = Vector2::Zero;

        auto keyboard = gameHost->GetKeyboard();
        auto scene = weakScene.lock();
        scene->connections += keyboard->KeyDown.Connect([this](Keys key) {
            if (key == Keys::Space) {
                auto nextScene = this->CreatePlayScene();
                sceneDirector.Transit(nextScene);
            }
        });

        for (auto & scoreTextLabel : scoreTextLabels) {
            auto component = scoreTextLabel.GetComponent<GraphicsComponent>();
            component->SetVisible(true);
        }
    };
    scene->OnUpdate = [this] {
        input1.Emit();
        input2.Emit();
    };
    scene->OnExit = [this] {
        for (auto & scoreTextLabel : scoreTextLabels) {
            auto component = scoreTextLabel.GetComponent<GraphicsComponent>();
            component->SetVisible(false);
        }
    };
    return scene;
}

std::shared_ptr<GameScene> PongGame::CreatePlayScene()
{
    auto scene = std::make_shared<GameScene>();
    scene->OnEnter = [this] {
        const float speed = 420.0f;
        std::mt19937 random(std::random_device{}());
        std::uniform_real_distribution<float> distribution(0.7f, 1.0f);
        static bool flipflop = false;
        flipflop = !flipflop;
        ball.Velocity = Vector2::Normalize(Vector2{
            (flipflop ? -1.0f: 1.0f), distribution(random)}) * speed;

        soundEffect2->Stop();
        soundEffect2->Play();
    };
    scene->OnUpdate = [this] {
        input1.Emit();
        input2.Emit();

        ball.PositionOld = ball.GetPosition();
        auto position = ball.GetPosition();
        position += ball.Velocity * clock->GetFrameDuration().count();
        ball.SetPosition(position);

        bool collision = false;

        if (position.X >= 0) {
            auto paddleCollider = paddle1.GetCollider();
            if (paddleCollider.Intersects(ball.GetCollider())
                && ball.PositionOld.X <= paddleCollider.Min.X) {
                ball.Velocity.X *= -1;
                collision = true;
            }
        }
        else {
            auto paddleCollider = paddle2.GetCollider();
            if (paddleCollider.Intersects(ball.GetCollider())
                && ball.PositionOld.X >= paddleCollider.Max.X) {
                ball.Velocity.X *= -1;
                collision = true;
            }
        }

        const auto halfWidth = gameFieldSize.Width / 2;
        const auto halfHeight = gameFieldSize.Height / 2;

        if (position.Y >= halfHeight || position.Y <= -halfHeight) {
            ball.Velocity.Y *= -1;
            position.Y = ball.PositionOld.Y;
            ball.SetPosition(position);
            collision = true;
        }

        const float offset = 70.0f;
        if (position.X >= (halfWidth + offset)) {
            player1.SetScore(player1.GetScore() + 1);
            soundEffect3->Stop();
            soundEffect3->Play();
            auto nextScene = this->CreateNewGameScene();
            sceneDirector.Transit(nextScene);
        }
        else if (position.X <= -(halfWidth + offset)) {
            player2.SetScore(player2.GetScore() + 1);
            soundEffect3->Stop();
            soundEffect3->Play();
            auto nextScene = this->CreateNewGameScene();
            sceneDirector.Transit(nextScene);
        }

        if (collision) {
            soundEffect1->Stop();
            soundEffect1->Play();
        }
    };
    return scene;
}

void PongGame::Update()
{
    gameEngine.Update();
    sceneDirector.Update();
}

void PongGame::Draw()
{
    gameEngine.Draw();
}

} // namespace Pong
