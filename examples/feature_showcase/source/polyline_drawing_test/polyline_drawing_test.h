#pragma once

#include "pomdog/experimental/graphics/polyline_batch.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

namespace feature_showcase {

using namespace pomdog;

class PolylineDrawingTest final : public Game {
public:
    explicit PolylineDrawingTest(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs);

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

    std::shared_ptr<PolylineBatch> lineBatch_;
    std::shared_ptr<PolylinePipeline> linePipeline_;
    std::vector<Vector2> path_;

    float lineWidth_ = 4.0f;
    bool polylineClosed_ = false;
};

} // namespace feature_showcase
