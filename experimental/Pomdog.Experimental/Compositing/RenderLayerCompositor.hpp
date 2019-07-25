// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

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

    void Draw(GraphicsCommandQueue & commandQueue, Renderer & renderer);

private:
    std::vector<std::shared_ptr<RenderLayer>> layers;
    bool needToSort;
};

} // namespace Pomdog
