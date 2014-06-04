//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "PrimitiveAxes.hpp"

namespace TestApp {
//-----------------------------------------------------------------------
PrimitiveAxes::PrimitiveAxes(std::shared_ptr<GameHost> const& gameHost)
{
	auto graphicsDevice = gameHost->GraphicsDevice();
	auto assets = gameHost->AssetManager();

	{
		using PositionColor = CustomVertex<Vector3, Vector3>;
		
		Color color1 {255, 149, 140, 255};
		Color color2 {170, 220, 170, 255};
		Color color3 {56, 190, 255, 255};
		
		std::array<PositionColor, 6> const verticesCombo = {
			// x axis
			Vector3(-1.0f, 0.0f, 0.0f), color1.ToVector3(),
			Vector3(+1.0f, 0.0f, 0.0f), color1.ToVector3(),
			// y axis
			Vector3(0.0f, -1.0f, 0.0f), color2.ToVector3(),
			Vector3(0.0f, +1.0f, 0.0f), color2.ToVector3(),
			// z axis
			Vector3(0.0f, 0.0f, -1.0f), color3.ToVector3(),
			Vector3(0.0f, 0.0f, +1.0f), color3.ToVector3(),
		};
		vertexBuffer = std::make_shared<VertexBuffer>(graphicsDevice,
			PositionColor::Declaration(), verticesCombo.data(), verticesCombo.size(), BufferUsage::Immutable);
	}
	{
		effectPass = assets->Load<EffectPass>("PrimitiveAxesEffect");
		inputLayout = std::make_shared<InputLayout>(graphicsDevice, effectPass);
	}
}
//-----------------------------------------------------------------------
void PrimitiveAxes::Draw(GraphicsContext & graphicsContext, Matrix4x4 const& transformMatrix)
{
	constexpr float axesPixelSize = 4096.0f;
	auto axesScaling = Matrix4x4::CreateScale({axesPixelSize, axesPixelSize, axesPixelSize});

	auto parameter = effectPass->Parameters("Matrices");
	parameter->SetValue(Matrix4x4::Transpose(axesScaling * transformMatrix));

	graphicsContext.SetInputLayout(inputLayout);
	graphicsContext.SetVertexBuffer(vertexBuffer);
	effectPass->Apply();
	graphicsContext.Draw(PrimitiveTopology::LineList);
}

}// namespace TestApp
