// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "RenderLayerCompositor.hpp"
#include "RenderLayer.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>

namespace Pomdog {

RenderLayerCompositor::RenderLayerCompositor()
    : needToSort(true)
{
}

void RenderLayerCompositor::AddLayer(const std::shared_ptr<RenderLayer>& layer)
{
    POMDOG_ASSERT(layer);
    POMDOG_ASSERT(std::end(layers) == std::find(std::begin(layers), std::end(layers), layer));
    layers.push_back(layer);
    needToSort = true;
}

void RenderLayerCompositor::RemoveLayer(const std::shared_ptr<RenderLayer>& layer)
{
    POMDOG_ASSERT(layer);
    layers.erase(std::remove(std::begin(layers), std::end(layers), layer), std::end(layers));
}

void RenderLayerCompositor::Draw(GraphicsCommandQueue& commandQueue, Renderer& renderer)
{
    if (needToSort) {
        std::sort(std::begin(layers), std::end(layers),
            [](const std::shared_ptr<RenderLayer>& a, const std::shared_ptr<RenderLayer>& b) {
                return a->DrawOrder() < b->DrawOrder();
            });

        needToSort = false;
    }

    for (auto& layer : layers) {
        POMDOG_ASSERT(layer);
        layer->Draw(commandQueue, renderer);
    }
}

} // namespace Pomdog
