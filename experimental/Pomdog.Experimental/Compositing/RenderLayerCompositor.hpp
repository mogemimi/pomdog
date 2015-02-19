//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_RENDERLAYERCOMPOSITOR_2AC2E2AD_BE41_41DE_B626_7831FE3A3F11_HPP
#define POMDOG_RENDERLAYERCOMPOSITOR_2AC2E2AD_BE41_41DE_B626_7831FE3A3F11_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>
#include <vector>
#include <memory>

namespace Pomdog {

class GraphicsContext;
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
	bool needSort;
};

}// namespace Pomdog

#endif // !defined(POMDOG_RENDERLAYERCOMPOSITOR_2AC2E2AD_BE41_41DE_B626_7831FE3A3F11_HPP)
