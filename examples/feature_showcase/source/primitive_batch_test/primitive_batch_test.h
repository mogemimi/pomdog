#pragma once

#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/pomdog.h"

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
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice;
    std::shared_ptr<gpu::CommandQueue> commandQueue;
    std::shared_ptr<gpu::CommandList> commandList;
    ConnectionList connect;

    std::shared_ptr<PrimitiveBatch> primitiveBatch;
    std::shared_ptr<Timer> timer;
};

} // namespace feature_showcase
