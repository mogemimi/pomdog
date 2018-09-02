#pragma once

#include <Pomdog.Experimental/Gameplay/Scene.hpp>
#include <Pomdog.Experimental/Gameplay2D/Simple2DGameEngine.hpp>
#include <Pomdog.Experimental/Gameplay2D/CameraComponent.hpp>
#include <Pomdog/Experimental/Graphics/SpriteFont.hpp>
#include <Pomdog/Pomdog.hpp>
#include <deque>
#include <map>

namespace Pong {

using namespace Pomdog;

struct Paddle {
    std::shared_ptr<Transform> transform;
    Vector2 PositionOld = Vector2::Zero;
    float Speed = 540.0f;
    float Height = 50.0f;

    Vector2 GetPosition() const
    {
        POMDOG_ASSERT(transform);
        return transform->GetPosition2D();
    }

    void SetPosition(const Vector2& positionIn)
    {
        POMDOG_ASSERT(transform);
        return transform->SetPosition2D(positionIn);
    }

    BoundingBox2D GetCollider() const
    {
        const float width = 10;
        const auto halfWidth = width / 2;
        const auto halfHeight = Height / 2;
        BoundingBox2D box;
        box.Min = GetPosition() - Vector2{halfWidth, halfHeight};
        box.Max = GetPosition() + Vector2{halfWidth, halfHeight};
        return box;
    }
};

struct Ball {
    std::shared_ptr<Transform> transform;
    Vector2 PositionOld = Vector2::Zero;
    Vector2 Velocity = Vector2::Zero;

    Vector2 GetPosition() const
    {
        POMDOG_ASSERT(transform);
        return transform->GetPosition2D();
    }

    void SetPosition(const Vector2& positionIn)
    {
        POMDOG_ASSERT(transform);
        return transform->SetPosition2D(positionIn);
    }

    BoundingCircle GetCollider() const
    {
        BoundingCircle circle;
        circle.Radius = 3;
        circle.Center = GetPosition();
        return circle;
    }
};

class Player {
private:
    int score;

public:
    Player()
        : score(0)
    {
    }

    int GetScore() const
    {
        return score;
    }

    void SetScore(int scoreIn)
    {
        score = scoreIn;
        ScoreChanged(score);
    }

    Signal<void(int score)> ScoreChanged;
};

class Input {
private:
    ConnectionList connections;
    ButtonState up = ButtonState::Released;
    ButtonState down = ButtonState::Released;
    Keys keyUp = Keys::W;
    Keys keyDown = Keys::S;

public:
    Signal<void()> Up;
    Signal<void()> Down;

    void Reset(Keys keyUpIn, Keys keyDownIn, std::shared_ptr<Keyboard> const& keyboard)
    {
        keyUp = keyUpIn;
        keyDown = keyDownIn;

        connections.Disconnect();
        auto & connect = connections;

        connect(keyboard->KeyUp, [this](Keys key) {
            POMDOG_ASSERT(keyUp != keyDown);
            if (key == keyUp) {
                up = ButtonState::Released;
            }
            else if (key == keyDown) {
                down = ButtonState::Released;
            }
        });

        connect(keyboard->KeyDown, [this](Keys key) {
            POMDOG_ASSERT(keyUp != keyDown);
            if (key == keyUp) {
                up = ButtonState::Pressed;
            }
            else if (key == keyDown) {
                down = ButtonState::Pressed;
            }
        });
    }

    void Emit()
    {
        int y = 0;
        if (up == ButtonState::Pressed) {
            ++y;
        }
        if (down == ButtonState::Pressed) {
            --y;
        }

        if (y > 0) {
            Up();
        } else if (y < 0) {
            Down();
        }
    }
};

class PongGame final : public Game {
public:
    explicit PongGame(const std::shared_ptr<GameHost>& gameHost);

    void Initialize() override;

    void Update() override;

    void Draw() override;

private:
    std::shared_ptr<GameScene> CreateNewGameScene();

    std::shared_ptr<GameScene> CreatePlayScene();

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<GameWindow> window;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<GraphicsCommandQueue> commandQueue;
    std::shared_ptr<AssetManager> assets;
    std::shared_ptr<GameClock> clock;
    std::shared_ptr<AudioEngine> audioEngine;

    Simple2DGameEngine gameEngine;

    std::shared_ptr<SoundEffect> soundEffect1;
    std::shared_ptr<SoundEffect> soundEffect2;
    std::shared_ptr<SoundEffect> soundEffect3;

    std::shared_ptr<SpriteFont> spriteFont;
    Timer textTimer;

    SceneDirector sceneDirector;
    Input input1;
    Input input2;
    Player player1;
    Player player2;
    Ball ball;
    Paddle paddle1;
    Paddle paddle2;
    Rectangle gameFieldSize;
    ConnectionList connect;
    std::vector<Entity> scoreTextLabels;
};

} // namespace Pong
