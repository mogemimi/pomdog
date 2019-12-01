// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstdint>
#include <memory>
#include <vector>

namespace Pomdog {

class RenderLayer;
class Renderer;

class RenderLayerCompositor {
public:
    RenderLayerCompositor();

    void AddLayer(const std::shared_ptr<RenderLayer>& layer);

    void RemoveLayer(const std::shared_ptr<RenderLayer>& layer);

    void Draw(GraphicsCommandQueue& commandQueue, Renderer& renderer);

private:
    std::vector<std::shared_ptr<RenderLayer>> layers;
    bool needToSort;
};

} // namespace Pomdog
