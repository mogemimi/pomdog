// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {

class ScreenQuad {
public:
    explicit ScreenQuad(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

    void DrawQuad(GraphicsCommandList & commandList);

private:
    std::shared_ptr<VertexBuffer> vertexBuffer;
};

} // namespace Pomdog
