// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_SCREENQUAD_8AD72196_HPP
#define POMDOG_SCREENQUAD_8AD72196_HPP

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

#endif // POMDOG_SCREENQUAD_8AD72196_HPP
