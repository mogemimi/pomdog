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
} // namespace pomdog::gpu

namespace feature_showcase {

using namespace pomdog;

/// ClearColorTest clears the back buffer to cornflower blue and draws nothing else.
class ClearColorTest final : public Game {
public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost) override;

    void update() override;

    void draw() override;

private:
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::shared_ptr<gpu::CommandQueue> commandQueue_;
    std::shared_ptr<gpu::CommandList> commandList_;
};

} // namespace feature_showcase
