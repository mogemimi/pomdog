#include "PongGame.hpp"
#include <Pomdog.Experimental/Graphics/TrueTypeFont.hpp>
#include <Pomdog.Experimental/ImageEffects/FXAA.hpp>
#include <Pomdog.Experimental/ImageEffects/VignetteEffect.hpp>
#include <Pomdog.Experimental/ImageEffects/FishEyeEffect.hpp>
#include <Pomdog.Experimental/ImageEffects/ChromaticAberration.hpp>
#include <Pomdog.Experimental/ImageEffects/RetroCrtEffect.hpp>
#include <utility>
#include <cmath>
#include <random>

namespace Pong {
//-----------------------------------------------------------------------
PongGame::PongGame(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , window(gameHostIn->Window())
    , graphicsDevice(gameHostIn->GraphicsDevice())
    , commandQueue(gameHostIn->GraphicsCommandQueue())
    , assets(gameHostIn->AssetManager())
    , clock(gameHostIn->Clock())
    , audioEngine(gameHostIn->AudioEngine())
    , spriteBatch(graphicsDevice, *assets)
    , polygonBatch(graphicsDevice, *assets)
    , postProcessCompositor(graphicsDevice,
        window->GetClientBounds().Width,
        window->GetClientBounds().Height,
        SurfaceFormat::R8G8B8A8_UNorm)
    , textTimer(clock)
{
    Log::Info("Hello");

    auto clientBounds = window->GetClientBounds();
    window->SetClientBounds({clientBounds.X, clientBounds.Y, 640, 512});
    window->SetAllowUserResizing(true);

    postProcessCompositor.SetViewportSize(*graphicsDevice,
        clientBounds.Width, clientBounds.Height);
}
//-----------------------------------------------------------------------
void PongGame::Initialize()
{
    // Set window name
    window->SetTitle("Pomdog Pong");

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

        paddle1.Position = {gameFieldSize.Width/2.0f, 0.0f};
        paddle2.Position = {-gameFieldSize.Width/2.0f, 0.0f};
        ball.Position = Vector2::Zero;
        player1.Score = 0;
        player2.Score = 0;

        auto keyboard = gameHost->Keyboard();

        input1.Reset(Keys::UpArrow, Keys::DownArrow, keyboard);
        input2.Reset(Keys::W, Keys::S, keyboard);

        connect(input1.Up, [this] {
            paddle1.PositionOld = paddle1.Position;
            paddle1.Position.Y += paddle1.Speed * clock->FrameDuration().count();

            if (paddle1.Position.Y > (gameFieldSize.Height / 2 - paddle1.Height / 2)) {
                paddle1.Position.Y = (gameFieldSize.Height / 2 - paddle1.Height / 2);
            }
        });
        connect(input1.Down, [this] {
            paddle1.PositionOld = paddle1.Position;
            paddle1.Position.Y -= paddle1.Speed * clock->FrameDuration().count();

            if (paddle1.Position.Y < -(gameFieldSize.Height / 2 - paddle1.Height / 2)) {
                paddle1.Position.Y = -(gameFieldSize.Height / 2 - paddle1.Height / 2);
            }
        });
        connect(input2.Up, [this] {
            paddle2.PositionOld = paddle2.Position;
            paddle2.Position.Y += paddle2.Speed * clock->FrameDuration().count();

            if (paddle2.Position.Y > (gameFieldSize.Height / 2 - paddle2.Height / 2)) {
                paddle2.Position.Y = (gameFieldSize.Height / 2 - paddle2.Height / 2);
            }
        });
        connect(input2.Down, [this] {
            paddle2.PositionOld = paddle2.Position;
            paddle2.Position.Y -= paddle2.Speed * clock->FrameDuration().count();

            if (paddle2.Position.Y < -(gameFieldSize.Height / 2 - paddle2.Height / 2)) {
                paddle2.Position.Y = -(gameFieldSize.Height / 2 - paddle2.Height / 2);
            }
        });

        textTimer.SetInterval(std::chrono::milliseconds(500));
        connect(textTimer.Elapsed, [this] {
            headerText = StringHelper::Format(
                "%.0lf sec\n%.0f fps   SCORE %d - %d",
                clock->TotalGameTime().count(),
                std::round(clock->FrameRate()),
                player1.Score,
                player2.Score);
        });
        bottomText = "[SPACE] to start, WS and Up/Down to move";
    }
    {
        PongState task;
        task.Enter = [this] {
            ball.Position = Vector2::Zero;
            ball.PositionOld = Vector2::Zero;
            ball.Velocity = Vector2::Zero;

            auto keyboard = gameHost->Keyboard();
            pressedStartConnection = keyboard->KeyDown.Connect([this](Keys key) {
                if (key == Keys::Space) {
                    pressedStartConnection.Disconnect();
                    scheduler.Transit("Play");
                }
            });
        };
        task.Update = [this] {
            input1.Emit();
            input2.Emit();
        };
        scheduler.Add("NewGame", task);
    }
    {
        PongState task;
        task.Enter = [this] {
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
        task.Update = [this] {
            input1.Emit();
            input2.Emit();

            ball.PositionOld = ball.Position;
            ball.Position += ball.Velocity * clock->FrameDuration().count();

            bool collision = false;

            if (ball.Position.X >= 0) {
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

            if (ball.Position.Y >= halfHeight || ball.Position.Y <= -halfHeight) {
                ball.Velocity.Y *= -1;
                ball.Position.Y = ball.PositionOld.Y;
                collision = true;
            }

            const float offset = 70.0f;
            if (ball.Position.X >= (halfWidth + offset)) {
                player1.Score += 1;
                soundEffect3->Stop();
                soundEffect3->Play();
                scheduler.Transit("NewGame");
            }
            else if (ball.Position.X <= -(halfWidth + offset)) {
                player2.Score += 1;
                soundEffect3->Stop();
                soundEffect3->Play();
                scheduler.Transit("NewGame");
            }

            if (collision) {
                soundEffect1->Stop();
                soundEffect1->Play();
            }
        };
        scheduler.Add("Play", task);
    }
    {
        scheduler.Transit("NewGame");
    }
    {
        //auto font = assets->Load<TrueTypeFont>("fonts/NotoSans-BoldItalic.ttf");
        auto font = std::make_shared<TrueTypeFont>(*assets, "fonts/NotoSans-BoldItalic.ttf");
        spriteFont = std::make_unique<SpriteFont>(graphicsDevice, font, '?', 26);
    }
    {
        auto fxaa = std::make_shared<FXAA>(graphicsDevice, *assets);
        auto fishEyeEffect = std::make_shared<FishEyeEffect>(graphicsDevice, *assets);
        auto vignetteEffect = std::make_shared<VignetteEffect>(graphicsDevice, *assets);
        auto chromaticAberration = std::make_shared<ChromaticAberration>(graphicsDevice, *assets);
        auto retroCrtEffect = std::make_shared<RetroCrtEffect>(graphicsDevice, *assets);

        vignetteEffect->SetIntensity(1.0f);
        fishEyeEffect->SetStrength(0.2f);

        postProcessCompositor.Composite({
            fxaa,
            retroCrtEffect,
            chromaticAberration,
            fishEyeEffect,
            vignetteEffect,
            chromaticAberration,
        });
    }
    {
        commandList = std::make_shared<GraphicsCommandList>(*graphicsDevice);

        auto onViewportChanged = [this](int width, int height) {
            viewport = {0, 0, width, height};

            renderTarget = std::make_shared<RenderTarget2D>(
                graphicsDevice, width, height, false,
                SurfaceFormat::R8G8B8A8_UNorm,
                DepthFormat::Depth24Stencil8);

            auto viewMatrix = Matrix4x4::CreateRotationX(MathHelper::ToRadians<float>(15.0f))
                * Matrix4x4::CreateLookAtLH(Vector3{0, 0, -512.0f}, Vector3::Zero, Vector3::UnitY);
            auto projectionMatrix = Matrix4x4::CreatePerspectiveFieldOfViewLH(
                MathHelper::ToRadians<float>(45.0f), viewport.GetAspectRatio(), 0.0f, 1000.0f);

            this->viewProjection = viewMatrix * projectionMatrix;


            postProcessCompositor.SetViewportSize(*graphicsDevice, width, height);
        };

        auto bounds = window->GetClientBounds();
        onViewportChanged(bounds.Width, bounds.Height);

        connect(window->ClientSizeChanged, onViewportChanged);
    }
}
//-----------------------------------------------------------------------
void PongGame::Update()
{
    scheduler.Update();
}
//-----------------------------------------------------------------------
void PongGame::Draw()
{
    // Reset graphics commands
    commandList->Reset();

    // Set the render target
    commandList->SetRenderTarget(renderTarget);
    commandList->SetViewport(viewport);
    commandList->SetScissorRectangle(viewport.GetBounds());

    // Fill background color
    commandList->Clear(ClearOptions::RenderTarget
        | ClearOptions::DepthBuffer
        | ClearOptions::Stencil, Color{32, 31, 30, 255}, 1.0f, 0);

    // Drawing polygon shapes
    polygonBatch.Begin(commandList, viewProjection);
    {
        // Rectangle outline
        auto p1 = Vector2(gameFieldSize.GetLeft(), gameFieldSize.GetBottom());
        auto p2 = Vector2(gameFieldSize.GetLeft(), gameFieldSize.GetTop());
        auto p3 = Vector2(gameFieldSize.GetRight(), gameFieldSize.GetTop());
        auto p4 = Vector2(gameFieldSize.GetRight(), gameFieldSize.GetBottom());
        polygonBatch.DrawLine(p1, p2, Color::White, 2.0f);
        polygonBatch.DrawLine(p2, p3, Color::White, 2.0f);
        polygonBatch.DrawLine(p3, p4, Color::White, 2.0f);
        polygonBatch.DrawLine(p4, p1, Color::White, 2.0f);

        // Dotted line
        int count = 32;
        float offset = 0.5f;
        float startY = -gameFieldSize.Height * 0.5f;
        float height = (gameFieldSize.Height + (gameFieldSize.Height / count * offset));
        for (int i = 0; i < count; ++i) {
            Vector2 start = {0.0f, height / count * i + startY};
            Vector2 end = {0.0f, height / count * (i + offset) + startY};
            polygonBatch.DrawLine(start, end, Color::White, 2.0f);
        }

        // Draw two paddles and ball
        polygonBatch.DrawRectangle(paddle1.Position, 10.0f, paddle1.Height, {0.5f, 0.5f}, Color::White);
        polygonBatch.DrawRectangle(paddle2.Position, 10.0f, paddle2.Height, {0.5f, 0.5f}, Color::White);
        polygonBatch.DrawCircle(ball.Position, 6.0f, 32, Color::White);
    }
    polygonBatch.End();

    // Font rendering
    spriteBatch.Begin(commandList, viewProjection);
    {
        if (scheduler.GetCurrentName() == "NewGame") {
            spriteFont->Draw(spriteBatch, "Player 1", Vector2(-135, 90), Color::Yellow);
            spriteFont->Draw(spriteBatch, "Player 2", Vector2(65, 90), Color::Yellow);
            spriteFont->Draw(spriteBatch, std::to_string(player1.Score), Vector2(-110, 50), Color::White, 0.0f, 2.0f);
            spriteFont->Draw(spriteBatch, std::to_string(player2.Score), Vector2(80, 50), Color::White, 0.0f, 2.0f);
        }
        spriteFont->Draw(spriteBatch, headerText, Vector2::Zero + Vector2(-110, 180), Color::White);
        spriteFont->Draw(spriteBatch, bottomText, Vector2::Zero + Vector2(-180, -160), Color::White);
    }
    spriteBatch.End();

    // Set the back buffer as the target
    commandList->SetRenderTarget();
    commandList->SetViewport(viewport);
    commandList->SetScissorRectangle(viewport.GetBounds());
    commandList->Clear(Color::CornflowerBlue);

    // Post processing
    postProcessCompositor.Draw(*commandList, renderTarget);

    // Close graphics command list
    commandList->Close();

    commandQueue->Reset();
    commandQueue->PushbackCommandList(commandList);
    commandQueue->ExecuteCommandLists();

    commandQueue->Present();
}
//-----------------------------------------------------------------------
} // namespace Pong
