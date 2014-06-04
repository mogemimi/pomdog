//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "LineRenderer.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
LineRenderer::LineRenderer(std::shared_ptr<GameHost> const& gameHost)
	: graphicsContext(gameHost->GraphicsContext())
{
	points.reserve(MaxLines * 2);
	
	auto graphicsDevice = gameHost->GraphicsDevice();
	auto assets = gameHost->AssetManager();

	{
		using PositionColor = CustomVertex<Vector3, Vector4>;
		points.resize(MaxLines * 2);
		
		vertexBuffer = std::make_shared<VertexBuffer>(graphicsDevice,
			PositionColor::Declaration(), points.data(), points.size(), BufferUsage::Dynamic);
	}
	{
		effectPass = assets->Load<EffectPass>("PrimitiveLineEffect");
		inputLayout = std::make_shared<InputLayout>(graphicsDevice, effectPass);
	}
}
//-----------------------------------------------------------------------
void LineRenderer::Begin(Matrix4x4 const& transformMatrix)
{
	points.clear();
	
	alignas(16) Matrix4x4 transposedMatrix = Matrix4x4::Transpose(transformMatrix);

	auto parameter = effectPass->Parameters("TransformMatrix");
	parameter->SetValue(transposedMatrix);
}
//-----------------------------------------------------------------------
void LineRenderer::Draw(Vector2 const& point1, Vector2 const& point2, Color const& color)
{
	Draw(Vector3{point1, 0.0f}, Vector3{point2, 0.0f}, color);
}
//-----------------------------------------------------------------------
void LineRenderer::Draw(Vector3 const& point1, Vector3 const& point2, Color const& color)
{
	POMDOG_ASSERT(points.size() / 2 < MaxLines);

	if (points.size() / 2 >= MaxLines) {
		///@todo Not implemented
		return;
	}

	auto colorVector = color.ToVector4();
	points.push_back({point1, colorVector});
	points.push_back({point2, colorVector});
}
//-----------------------------------------------------------------------
void LineRenderer::End()
{
	vertexBuffer->SetData(points.data(), static_cast<std::uint32_t>(points.size()));

	graphicsContext->SetInputLayout(inputLayout);
	graphicsContext->SetVertexBuffer(vertexBuffer);
	effectPass->Apply();
	graphicsContext->Draw(PrimitiveTopology::LineList, static_cast<std::uint32_t>(points.size()));
}
//-----------------------------------------------------------------------
}// namespace Pomdog
