//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "ScreenQuad.hpp"
#include "Pomdog/Graphics/ConstantBufferBinding.hpp"
#include "Pomdog/Graphics/CustomVertex.hpp"
#include "Pomdog/Graphics/GraphicsContext.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/PrimitiveTopology.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <array>

namespace Pomdog {
//-----------------------------------------------------------------------
ScreenQuad::ScreenQuad(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	using ScreenQuadVertex = CustomVertex<Vector3, Vector2>;
	
	std::array<ScreenQuadVertex, 4> const verticesCombo = {
		Vector3{-1.0f, -1.0f, 0.5f}, Vector2{0.0f, 0.0f},
		Vector3{-1.0f,  1.0f, 0.5f}, Vector2{0.0f, 1.0f},
		Vector3{ 1.0f,  1.0f, 0.5f}, Vector2{1.0f, 1.0f},
		Vector3{ 1.0f, -1.0f, 0.5f}, Vector2{1.0f, 0.0f},
	};
	
	std::array<ScreenQuadVertex, 6> const vertices = {
		verticesCombo[0], verticesCombo[1], verticesCombo[2],
		verticesCombo[2], verticesCombo[3], verticesCombo[0],
	};
	
	vertexBuffer = std::make_shared<VertexBuffer>(graphicsDevice,
		vertices.data(), vertices.size(), sizeof(ScreenQuadVertex), BufferUsage::Immutable);
}
//-----------------------------------------------------------------------
void ScreenQuad::DrawQuad(GraphicsContext & graphicsContext)
{
	graphicsContext.SetVertexBuffer(vertexBuffer);
	graphicsContext.Draw(PrimitiveTopology::TriangleList, vertexBuffer->VertexCount());
}
//-----------------------------------------------------------------------
}// namespace Pomdog
