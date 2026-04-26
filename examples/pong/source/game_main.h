#pragma once

#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/experimental/graphics/sprite_font.h"
#include "pomdog/experimental/graphics/truetype_font.h"
#include "pomdog/experimental/image_effects/post_process_compositor.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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
    std::shared_ptr<Keyboard> keyboard_;
    Keys keyUp = Keys::W;
    Keys keyDown = Keys::S;

public:
    Signal<void()> Up;
    Signal<void()> Down;

    void Reset(Keys keyUpIn, Keys keyDownIn, std::shared_ptr<Keyboard> const& keyboard)
    {
        keyUp = keyUpIn;
        keyDown = keyDownIn;
        keyboard_ = keyboard;
    }

    void Emit()
    {
        int y = 0;
        if (keyboard_ && keyboard_->isKeyDown(keyUp)) {
            ++y;
        }
        if (keyboard_ && keyboard_->isKeyDown(keyDown)) {
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
    explicit GameMain(std::shared_ptr<vfs::FileSystemContext> fs);

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost) override;

    void update() override;

    void draw() override;

private:
    std::shared_ptr<GameHost> gameHost_;
    std::shared_ptr<GameWindow> window_;
    std::shared_ptr<vfs::FileSystemContext> fs_;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::shared_ptr<gpu::CommandQueue> commandQueue_;
    std::shared_ptr<gpu::CommandList> commandList_;
    std::shared_ptr<GameClock> clock_;
    std::shared_ptr<AudioEngine> audioEngine_;

    std::shared_ptr<SpriteBatch> spriteBatch_;
    std::shared_ptr<SpritePipeline> spritePipeline_;
    std::shared_ptr<SpriteFont> spriteFont_;
    std::shared_ptr<PrimitiveBatch> primitiveBatch_;
    std::shared_ptr<PrimitivePipeline> primitivePipeline_;
    std::shared_ptr<gpu::RenderTarget2D> renderTarget_;
    std::shared_ptr<gpu::DepthStencilBuffer> depthStencilBuffer_;
    std::unique_ptr<PostProcessCompositor> postProcessCompositor_;

    std::shared_ptr<AudioSource> audioSource1_;
    std::shared_ptr<AudioSource> audioSource2_;
    std::shared_ptr<AudioSource> audioSource3_;

    PongScenes pongScene_;
    Input input1_;
    Input input2_;
    Player player1_;
    Player player2_;
    Ball ball_;
    Paddle paddle1_;
    Paddle paddle2_;
    Rect2D gameFieldSize_;
    bool scoreTextVisible_ = false;
    std::string headerText_;
    std::unique_ptr<Timer> textTimer_;
    ConnectionList connect_;
};

} // namespace pong
