// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_RENDERLAYER_925B26B2_1D0D_4A78_9A84_DBE025AD0BA3_HPP
#define POMDOG_RENDERLAYER_925B26B2_1D0D_4A78_9A84_DBE025AD0BA3_HPP

#include <cstdint>

namespace Pomdog {

class GraphicsContext;
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

}// namespace Pomdog

#endif // !defined(POMDOG_RENDERLAYER_925B26B2_1D0D_4A78_9A84_DBE025AD0BA3_HPP)
