// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {

class ScreenQuad {
public:
    explicit ScreenQuad(const std::shared_ptr<GraphicsDevice>& graphicsDevice);

    void DrawQuad(GraphicsCommandList & commandList);

private:
    std::shared_ptr<VertexBuffer> vertexBuffer;
};

} // namespace Pomdog
