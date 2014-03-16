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
		
		std::array<PositionColor, 6> const verticesCombo = {
			// x axis
			Vector3(-1.0f, 0.0f, 0.0f), Vector3(255.0f/255.0f, 49.0f/255.0f, 60.0f/255.0f),
			Vector3(+1.0f, 0.0f, 0.0f), Vector3(255.0f/255.0f, 49.0f/255.0f, 60.0f/255.0f),
			// y axis
			Vector3(0.0f, -1.0f, 0.0f), Vector3(170.0f/255.0f, 220.0f/255.0f, 170.0f/255.0f),
			Vector3(0.0f, +1.0f, 0.0f), Vector3(170.0f/255.0f, 220.0f/255.0f, 170.0f/255.0f),
			// z axis
			Vector3(0.0f, 0.0f, -1.0f), Vector3(56.0f/255.0f, 190.0f/255.0f, 255.0f/255.0f),
			Vector3(0.0f, 0.0f, +1.0f), Vector3(56.0f/255.0f, 190.0f/255.0f, 255.0f/255.0f),
		};
		vertexBuffer = std::make_shared<ImmutableVertexBuffer>(graphicsDevice,
			PositionColor::Declaration(), verticesCombo.data(), verticesCombo.size());
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
	parameter->SetValue((axesScaling * transformMatrix).Transpose());

	graphicsContext.SetInputLayout(inputLayout);
	graphicsContext.SetVertexBuffer(vertexBuffer);
	effectPass->Apply();
	graphicsContext.Draw(PrimitiveTopology::LineList);
}

}// namespace TestApp
