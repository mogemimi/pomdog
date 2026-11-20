// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class CommandList;
class CommandQueue;
class GraphicsDevice;
class PipelineState;
class VertexBuffer;
} // namespace pomdog::gpu

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace feature_showcase {

using namespace pomdog;

/// TriangleTest draws one triangle with a red, a green, and a blue vertex,
/// using a vertex buffer, a pair of shaders, and a pipeline state directly.
class TriangleTest final : public Game {
public:
    explicit TriangleTest(const std::shared_ptr<vfs::FileSystemContext>& fs);

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost) override;

    void update() override;

    void draw() override;

private:
    std::shared_ptr<vfs::FileSystemContext> fs_;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::shared_ptr<gpu::CommandQueue> commandQueue_;
    std::shared_ptr<gpu::CommandList> commandList_;
    std::shared_ptr<gpu::VertexBuffer> vertexBuffer_;
    std::shared_ptr<gpu::PipelineState> pipelineState_;
};

} // namespace feature_showcase
