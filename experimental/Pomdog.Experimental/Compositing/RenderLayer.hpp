// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstdint>

namespace Pomdog {

class Renderer;

class RenderLayer {
public:
    virtual ~RenderLayer() = default;

    virtual void Draw(GraphicsCommandQueue & commandQueue, Renderer & renderer) = 0;

    std::int32_t DrawOrder() const;

    void DrawOrder(std::int32_t drawOrder);

private:
    std::int32_t drawOrder = 0;
};

} // namespace Pomdog
