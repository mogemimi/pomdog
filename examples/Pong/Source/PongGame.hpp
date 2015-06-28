#ifndef PONGGAME_HPP
#define PONGGAME_HPP

#include <Pomdog.Experimental/Graphics/PolygonBatch.hpp>
#include <Pomdog.Experimental/Graphics/SpriteBatchRenderer.hpp>
#include <Pomdog.Experimental/Graphics/SpriteFont.hpp>
#include <Pomdog.Experimental/ImageEffects/PostProcessCompositor.hpp>
#include <Pomdog/Pomdog.hpp>
#include <deque>
#include <map>

namespace Pong {

using namespace Pomdog;

struct Paddle {
    Vector2 Position = Vector2::Zero;
    Vector2 PositionOld = Vector2::Zero;
    float Speed = 540.0f;
    float Height = 50.0f;

    BoundingBox2D GetCollider() const
    {
        const float width = 10;
        const auto halfWidth = width / 2;
        const auto halfHeight = Height / 2;
        BoundingBox2D box;
        box.Min = Position - Vector2{halfWidth, halfHeight};
        box.Max = Position + Vector2{halfWidth, halfHeight};
        return std::move(box);
    }
};

struct Ball {
    Vector2 Position = Vector2::Zero;
    Vector2 PositionOld = Vector2::Zero;
    Vector2 Velocity = Vector2::Zero;

    BoundingCircle GetCollider() const
    {
        BoundingCircle circle;
        circle.Radius = 3;
        circle.Center = Position;
        return std::move(circle);
    }
};

struct Player {
    int Score;
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
        connections.Connect(keyboard->KeyUp, [this](Keys key) {
            POMDOG_ASSERT(keyUp != keyDown);
            if (key == keyUp) {
                up = ButtonState::Released;
            }
            else if (key == keyDown) {
                down = ButtonState::Released;
            }
        });

        connections.Connect(keyboard->KeyDown, [this](Keys key) {
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

struct PongState {
    std::function<void()> Enter;
    std::function<void()> Update;
    std::function<void()> Exit;
};

using PongTask = std::function<void()>;

class PongScheduler {
private:
    std::vector<PongTask> taskQueue;
    std::map<std::string, PongState> states;
    std::string currentTaskName;

public:
    void Transit(std::string const& name)
    {
        auto oldTaskName = currentTaskName;
        currentTaskName = name;

        {
            auto taskIter = states.find(oldTaskName);
            if (taskIter != std::end(states)) {
                auto & task = taskIter->second;
                taskQueue.push_back(task.Exit);
            }
        }
        {
            auto taskIter = states.find(currentTaskName);
            if (taskIter != std::end(states)) {
                auto & task = taskIter->second;
                taskQueue.push_back(task.Enter);
                taskQueue.push_back(task.Update);
            }
        }
    }

    void Add(std::string const& name, PongState const& task)
    {
        states.emplace(name, task);
    }

    void Update()
    {
        std::vector<PongTask> temporaryQueue;
        std::swap(taskQueue, temporaryQueue);

        for (auto & task: temporaryQueue) {
            if (task) {
                task();
            }
        }

        auto taskIter = states.find(currentTaskName);
        if (taskIter != std::end(states)) {
            auto & task = taskIter->second;
            taskQueue.push_back(task.Update);
        }
    }

    std::string GetCurrentName() const
    {
        return currentTaskName;
    }
};

class PongGame : public Game {
public:
    explicit PongGame(const std::shared_ptr<GameHost>& gameHost);

    void Initialize() override;

    void Update() override;

    void Draw() override;

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<GameWindow> window;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<AssetManager> assets;
    std::shared_ptr<GameClock> clock;
    std::shared_ptr<AudioEngine> audioEngine;

    std::shared_ptr<SoundEffect> soundEffect1;
    std::shared_ptr<SoundEffect> soundEffect2;
    std::shared_ptr<SoundEffect> soundEffect3;

    std::shared_ptr<GraphicsCommandQueue> commandQueue;
    std::shared_ptr<GraphicsCommandList> commandList;
    std::shared_ptr<RenderTarget2D> renderTarget;

    Matrix4x4 viewProjection;
    PostProcessCompositor postProcessCompositor;
    SpriteBatchRenderer spriteBatch;
    PolygonBatch polygonBatch;

    std::unique_ptr<SpriteFont> spriteFont;
    std::string headerText;
    std::string bottomText;
    Timer textTimer;

    PongScheduler scheduler;
    Input input1;
    Input input2;
    Player player1;
    Player player2;
    Ball ball;
    Paddle paddle1;
    Paddle paddle2;
    Viewport viewport;
    Rectangle gameFieldSize;
    ConnectionList connect;
    Connection pressedStartConnection;
};

} // namespace Pong

#endif // PONGGAME_HPP
