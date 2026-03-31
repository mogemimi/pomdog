#pragma once

#include "pomdog/experimental/graphics/line_batch.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

namespace feature_showcase {

using namespace pomdog;

class LineBatchTest final : public Game {
public:
    explicit LineBatchTest(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs);

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost, int argc, const char* const* argv) override;

    void update() override;

    void draw() override;

private:
    std::shared_ptr<GameHost> gameHost_;
    std::shared_ptr<vfs::FileSystemContext> fs_;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::shared_ptr<gpu::CommandQueue> commandQueue_;
    std::shared_ptr<gpu::CommandList> commandList_;
    ConnectionList connect_;

    std::shared_ptr<LineBatch> lineBatch_;
    std::shared_ptr<LineBatch> lineBatch2_;
    std::shared_ptr<LinePipeline> linePipeline_;
    std::shared_ptr<Timer> timer_;
    std::vector<Vector2> path_;
};

} // namespace feature_showcase
