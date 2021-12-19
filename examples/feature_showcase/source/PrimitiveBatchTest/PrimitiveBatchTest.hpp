#pragma once

#include <pomdog/experimental/graphics/primitive_batch.hpp>
#include <pomdog/pomdog.hpp>

namespace feature_showcase {

using namespace pomdog;

class PrimitiveBatchTest final : public Game {
public:
    explicit PrimitiveBatchTest(const std::shared_ptr<GameHost>& gameHost);

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

    std::shared_ptr<PrimitiveBatch> primitiveBatch;
    std::shared_ptr<Timer> timer;
};

} // namespace feature_showcase
