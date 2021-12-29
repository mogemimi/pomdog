#pragma once

#include "pomdog/experimental/graphics/line_batch.h"
#include "pomdog/pomdog.h"

namespace feature_showcase {

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
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice;
    std::shared_ptr<gpu::CommandQueue> commandQueue;
    std::shared_ptr<gpu::CommandList> commandList;
    ConnectionList connect;

    std::shared_ptr<LineBatch> lineBatch;
    std::shared_ptr<LineBatch> lineBatch2;
    std::shared_ptr<Timer> timer;
    std::vector<Vector2> path;
};

} // namespace feature_showcase
