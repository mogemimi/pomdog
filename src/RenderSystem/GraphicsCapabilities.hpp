// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GRAPHICSCAPABILITIES_17FEAA5D_4BF2_4EEB_8D34_77B032B1534B_HPP
#define POMDOG_GRAPHICSCAPABILITIES_17FEAA5D_4BF2_4EEB_8D34_77B032B1534B_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstddef>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class GraphicsCapabilities {
public:
	std::size_t SamplerSlotCount;
};

}// namespace RenderSystems
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_GRAPHICSCAPABILITIES_17FEAA5D_4BF2_4EEB_8D34_77B032B1534B_HPP)
