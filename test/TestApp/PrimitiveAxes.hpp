//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_PRIMITIVEAXES_B7717D29_25D2_44E7_8F48_2642088D2B41_HPP
#define POMDOG_PRIMITIVEAXES_B7717D29_25D2_44E7_8F48_2642088D2B41_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>

namespace TestApp {

using namespace Pomdog;

class PrimitiveAxes
{
public:
	explicit PrimitiveAxes(std::shared_ptr<GameHost> const& gameHost);
	
	void Draw(Matrix4x4 const& transformMatrix);
	
private:
	std::shared_ptr<GraphicsContext> graphicsContext;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<EffectPass> effectPass;
	std::shared_ptr<InputLayout> inputLayout;
};

}// namespace TestApp

#endif // !defined(POMDOG_PRIMITIVEAXES_B7717D29_25D2_44E7_8F48_2642088D2B41_HPP)
