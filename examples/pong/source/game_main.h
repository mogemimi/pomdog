#pragma once

#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/experimental/graphics/sprite_font.h"
#include "pomdog/experimental/graphics/truetype_font.h"
#include "pomdog/experimental/graphics/truetype_font_loader.h"
#include "pomdog/experimental/image_effects/post_process_compositor.h"
#include "pomdog/pomdog.h"
#include <functional>
#include <memory>
#include <string>

namespace pong {

using namespace pomdog;

struct Paddle final {
    Vector2 position = Vector2::createZero();
    Vector2 positionOld = Vector2::createZero();
    float speed = 540.0f;
    float height = 50.0f;

    BoundingBox2D GetCollider() const
    {
        const float width = 10;
        const auto halfWidth = width / 2;
        const auto halfHeight = height / 2;
        BoundingBox2D box;
        box.min = position - Vector2{halfWidth, halfHeight};
        box.max = position + Vector2{halfWidth, halfHeight};
        return box;
    }
};

struct Ball final {
    Vector2 position = Vector2::createZero();
    Vector2 positionOld = Vector2::createZero();
    Vector2 velocity = Vector2::createZero();

    BoundingCircle GetCollider() const
    {
        BoundingCircle circle;
        circle.radius = 3;
        circle.center = position;
        return circle;
    }
};

class Player final {
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

class Input final {
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

        connections.disconnect();
        auto& connect = connections;

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
        }
        else if (y < 0) {
            Down();
        }
    }
};

enum class PongScenes {
    StartWaiting,
    Waiting,
    Prepare,
    Playing,
    GameOver,
};

class GameMain final : public Game {
public:
    explicit GameMain(const std::shared_ptr<GameHost>& gameHost);

    [[nodiscard]] std::unique_ptr<Error>
    initialize() override;

    void update() override;

    void draw() override;

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<GameWindow> window;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice;
    std::shared_ptr<gpu::CommandQueue> commandQueue;
    std::shared_ptr<gpu::CommandList> commandList;
    std::shared_ptr<AssetManager> assets;
    std::shared_ptr<GameClock> clock;
    std::shared_ptr<AudioEngine> audioEngine;

    std::shared_ptr<SpriteBatch> spriteBatch;
    std::shared_ptr<SpriteFont> spriteFont;
    std::shared_ptr<PrimitiveBatch> primitiveBatch;
    std::shared_ptr<gpu::RenderTarget2D> renderTarget;
    std::shared_ptr<gpu::DepthStencilBuffer> depthStencilBuffer;
    PostProcessCompositor postProcessCompositor;

    std::shared_ptr<SoundEffect> soundEffect1;
    std::shared_ptr<SoundEffect> soundEffect2;
    std::shared_ptr<SoundEffect> soundEffect3;

    PongScenes pongScene;
    Input input1;
    Input input2;
    Player player1;
    Player player2;
    Ball ball;
    Paddle paddle1;
    Paddle paddle2;
    Rectangle gameFieldSize;
    ScopedConnection startButtonConn;
    bool scoreTextVisible = false;
    std::string headerText;
    Timer textTimer;
    ConnectionList connect;
};

} // namespace pong
