#pragma once

#include <Pomdog/Experimental/Graphics/LineBatch.hpp>
#include <Pomdog/Pomdog.hpp>

namespace FeatureShowcase {

using namespace Pomdog;

class LineBatchTest final : public Game {
public:
    explicit LineBatchTest(const std::shared_ptr<GameHost>& gameHost);

    void Initialize() override;

    void Update() override;

    void Draw() override;

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<GraphicsCommandQueue> commandQueue;
    std::shared_ptr<GraphicsCommandList> commandList;
    ConnectionList connect;

    std::shared_ptr<LineBatch> lineBatch;
    std::shared_ptr<LineBatch> lineBatch2;
    std::shared_ptr<Timer> timer;
    std::vector<Vector2> path;
};

} // namespace FeatureShowcase
