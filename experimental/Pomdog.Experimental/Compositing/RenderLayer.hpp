// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_RENDERLAYER_925B26B2_HPP
#define POMDOG_RENDERLAYER_925B26B2_HPP

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstdint>

namespace Pomdog {

class Renderer;

class RenderLayer {
public:
    virtual ~RenderLayer() = default;

    virtual void Draw(GraphicsContext & graphicsContext, Renderer & renderer) = 0;

    std::int32_t DrawOrder() const;

    void DrawOrder(std::int32_t drawOrder);

private:
    std::int32_t drawOrder = 0;
};

} // namespace Pomdog

#endif // POMDOG_RENDERLAYER_925B26B2_HPP
