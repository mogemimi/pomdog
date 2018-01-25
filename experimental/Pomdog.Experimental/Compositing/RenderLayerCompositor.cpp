// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "RenderLayerCompositor.hpp"
#include "RenderLayer.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>

namespace Pomdog {

RenderLayerCompositor::RenderLayerCompositor()
    : needToSort(true)
{}

void RenderLayerCompositor::AddLayer(std::shared_ptr<RenderLayer> const& layer)
{
    POMDOG_ASSERT(layer);
    POMDOG_ASSERT(std::end(layers) == std::find(std::begin(layers), std::end(layers), layer));
    layers.push_back(layer);
    needToSort = true;
}

void RenderLayerCompositor::RemoveLayer(std::shared_ptr<RenderLayer> const& layer)
{
    POMDOG_ASSERT(layer);
    layers.erase(std::remove(std::begin(layers), std::end(layers), layer), std::end(layers));
}

void RenderLayerCompositor::Draw(GraphicsCommandQueue & commandQueue, Renderer & renderer)
{
    if (needToSort) {
        std::sort(std::begin(layers), std::end(layers),
            [](std::shared_ptr<RenderLayer> const& a, std::shared_ptr<RenderLayer> const& b){
                return a->DrawOrder() < b->DrawOrder();
            });

        needToSort = false;
    }

    for (auto & layer: layers) {
        POMDOG_ASSERT(layer);
        layer->Draw(commandQueue, renderer);
    }
}

} // namespace Pomdog
