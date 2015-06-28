// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_RENDERLAYERCOMPOSITOR_2AC2E2AD_HPP
#define POMDOG_RENDERLAYERCOMPOSITOR_2AC2E2AD_HPP

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstdint>
#include <vector>
#include <memory>

namespace Pomdog {

class RenderLayer;
class Renderer;

class RenderLayerCompositor {
public:
    RenderLayerCompositor();

    void AddLayer(std::shared_ptr<RenderLayer> const& layer);

    void RemoveLayer(std::shared_ptr<RenderLayer> const& layer);

    void Draw(GraphicsContext & graphicsContext, Renderer & renderer);

private:
    std::vector<std::shared_ptr<RenderLayer>> layers;
    bool needToSort;
};

} // namespace Pomdog

#endif // POMDOG_RENDERLAYERCOMPOSITOR_2AC2E2AD_HPP
