#pragma once

#include <pomdog/experimental/graphics/line_batch.hpp>
#include <pomdog/pomdog.hpp>

namespace FeatureShowcase {

using namespace pomdog;

class LineBatchTest final : public Game {
public:
    explicit LineBatchTest(const std::shared_ptr<GameHost>& gameHost);

    [[nodiscard]] std::unique_ptr<Error>
    Initialize() override;

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
