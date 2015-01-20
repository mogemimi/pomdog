//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "RenderLayerCompositor.hpp"
#include "RenderLayer.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>

namespace Pomdog {
//-----------------------------------------------------------------------
RenderLayerCompositor::RenderLayerCompositor()
	: needSort(true)
{}
//-----------------------------------------------------------------------
void RenderLayerCompositor::AddLayer(std::shared_ptr<RenderLayer> const& layer)
{
	POMDOG_ASSERT(layer);
	POMDOG_ASSERT(std::end(layers) == std::find(std::begin(layers), std::end(layers), layer));
	layers.push_back(layer);
	needSort = true;
}
//-----------------------------------------------------------------------
void RenderLayerCompositor::RemoveLayer(std::shared_ptr<RenderLayer> const& layer)
{
	POMDOG_ASSERT(layer);
	layers.erase(std::remove(std::begin(layers), std::end(layers), layer), std::end(layers));
}
//-----------------------------------------------------------------------
void RenderLayerCompositor::Draw(GraphicsContext & graphicsContext, Renderer & renderer)
{
	if (needSort)
	{
		std::sort(std::begin(layers), std::end(layers),
			[](std::shared_ptr<RenderLayer> const& a, std::shared_ptr<RenderLayer> const& b){
				return a->DrawOrder() < b->DrawOrder();
			});

		needSort = false;
	}

	for (auto & layer: layers)
	{
		POMDOG_ASSERT(layer);
		layer->Draw(graphicsContext, renderer);
	}
}
//-----------------------------------------------------------------------
}// namespace Pomdog
