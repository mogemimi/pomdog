// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class CommandList;
class GraphicsDevice;
class VertexBuffer;
} // namespace pomdog::gpu

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog {

class POMDOG_EXPORT ScreenQuad final {
private:
    std::shared_ptr<gpu::VertexBuffer> vertexBuffer_;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice);

    void drawQuad(gpu::CommandList& commandList);
};

} // namespace pomdog
