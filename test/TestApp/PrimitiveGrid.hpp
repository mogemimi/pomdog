//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_PRIMITIVEGRID_8E899444_553D_4CCC_87EB_11D70DFEC99C_HPP
#define POMDOG_PRIMITIVEGRID_8E899444_553D_4CCC_87EB_11D70DFEC99C_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>

namespace TestApp {

using namespace Pomdog;

class PrimitiveGrid
{
public:
	explicit PrimitiveGrid(std::shared_ptr<GameHost> const& gameHost);
	
	void Draw(GraphicsContext & graphicsContext, Matrix4x4 const& transformMatrix);
	
private:
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<EffectPass> effectPass;
	std::shared_ptr<InputLayout> inputLayout;
};

}// namespace TestApp

#endif // !defined(POMDOG_PRIMITIVEGRID_8E899444_553D_4CCC_87EB_11D70DFEC99C_HPP)
