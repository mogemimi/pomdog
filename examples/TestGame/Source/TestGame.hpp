#pragma once

#include <Pomdog.Experimental/Experimental.hpp>
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

struct alignas(16) MyShaderConstants {
    Matrix4x4 Model;
    Matrix4x4 ViewProjection;
};

class EventStreamKeyboard {
private:
    ScopedConnection connection;
    std::shared_ptr<GameClock> clock;
    Signal<void()> onCompleted;
    bool enabled;

public:
    explicit EventStreamKeyboard(const std::shared_ptr<GameHost>& gameHost)
        : EventStreamKeyboard(gameHost->GetClock())
    {}

    explicit EventStreamKeyboard(const std::shared_ptr<GameClock>& clockIn)
        : clock(clockIn)
    {}

    void SetEnable(bool enableIn)
    {
        if (this->enabled == enableIn) {
            return;
        }
        enabled = enableIn;
        if (enabled) {
            connection = clock->OnTick.Connect([this](auto) {
                onCompleted();
            });
        } else {
            connection.Disconnect();
        }
    }

    Connection Subscribe(std::function<void()> callback)
    {
        return onCompleted.Connect(callback);
    }
};

class TestGame : public Game {
public:
    explicit TestGame(const std::shared_ptr<GameHost>& gameHost);

    void Initialize() override;

    void Update() override;

    void Draw() override;

private:
    std::shared_ptr<GameHost> gameHost;
    Simple2DGameEngine gameEngine;
    ConnectionList connect;
    EventStreamKeyboard inputStream;
};

} // namespace Pomdog
