// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class POMDOG_EXPORT ScreenQuad final {
public:
    explicit ScreenQuad(const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice);

    void DrawQuad(gpu::CommandList& commandList);

private:
    std::shared_ptr<gpu::VertexBuffer> vertexBuffer;
};

} // namespace pomdog
