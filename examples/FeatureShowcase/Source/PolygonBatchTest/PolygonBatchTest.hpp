#pragma once

#include <Pomdog/Pomdog.hpp>
#include <Pomdog/Experimental/Graphics/PolygonBatch.hpp>

namespace FeatureShowcase {

using namespace Pomdog;

class PolygonBatchTest final : public Game {
public:
    explicit PolygonBatchTest(const std::shared_ptr<GameHost>& gameHost);

    void Initialize() override;

    void Update() override;

    void Draw() override;

private:
    std::shared_ptr<GameHost> gameHost;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<GraphicsCommandQueue> commandQueue;
    std::shared_ptr<GraphicsCommandList> commandList;
    ConnectionList connect;

    std::shared_ptr<PolygonBatch> polygonBatch;
    std::shared_ptr<Timer> timer;
};

} // namespace FeatureShowcase
