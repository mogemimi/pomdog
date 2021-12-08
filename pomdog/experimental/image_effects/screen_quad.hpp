// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/graphics/forward_declarations.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class POMDOG_EXPORT ScreenQuad final {
public:
    explicit ScreenQuad(const std::shared_ptr<GraphicsDevice>& graphicsDevice);

    void DrawQuad(GraphicsCommandList& commandList);

private:
    std::shared_ptr<VertexBuffer> vertexBuffer;
};

} // namespace pomdog
