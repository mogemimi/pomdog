// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

class POMDOG_EXPORT ScreenQuad final {
public:
    explicit ScreenQuad(const std::shared_ptr<GraphicsDevice>& graphicsDevice);

    void DrawQuad(GraphicsCommandList& commandList);

private:
    std::shared_ptr<VertexBuffer> vertexBuffer;
};

} // namespace Pomdog
