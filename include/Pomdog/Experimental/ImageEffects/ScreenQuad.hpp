// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT ScreenQuad final {
public:
    explicit ScreenQuad(const std::shared_ptr<GraphicsDevice>& graphicsDevice);

    void DrawQuad(GraphicsCommandList& commandList);

private:
    std::shared_ptr<VertexBuffer> vertexBuffer;
};

} // namespace Pomdog
