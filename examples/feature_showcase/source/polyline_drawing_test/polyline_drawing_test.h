#pragma once

#include "pomdog/experimental/graphics/polyline_batch.h"
#include "pomdog/pomdog.h"

namespace feature_showcase {

using namespace pomdog;

class PolylineDrawingTest final : public Game {
public:
    explicit PolylineDrawingTest(const std::shared_ptr<GameHost>& gameHost);

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

    std::shared_ptr<PolylineBatch> lineBatch;
    std::vector<Vector2> path;

    float lineWidth = 4.0f;
    bool polylineClosed = false;
};

} // namespace feature_showcase
