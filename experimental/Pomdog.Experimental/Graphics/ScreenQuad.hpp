//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SCREENQUAD_8AD72196_97F0_42DC_8EBC_0E89DC8D1FB9_HPP
#define POMDOG_SCREENQUAD_8AD72196_97F0_42DC_8EBC_0E89DC8D1FB9_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {

class ScreenQuad {
public:
	explicit ScreenQuad(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	void DrawQuad(GraphicsContext & graphicsContext);

private:
	std::shared_ptr<VertexBuffer> vertexBuffer;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SCREENQUAD_8AD72196_97F0_42DC_8EBC_0E89DC8D1FB9_HPP)
