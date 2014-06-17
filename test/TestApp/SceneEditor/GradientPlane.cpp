//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "GradientPlane.hpp"

namespace Pomdog {
namespace SceneEditor {
//-----------------------------------------------------------------------
GradientPlane::GradientPlane(std::shared_ptr<GameHost> const& gameHost)
	: graphicsContext(gameHost->GraphicsContext())
{
	auto graphicsDevice = gameHost->GraphicsDevice();
	auto assets = gameHost->AssetManager();

	{
		using PositionColor = CustomVertex<Vector3, Vector4>;
		
		Color colorUpper {90, 86, 80, 255};
		Color colorLower {57, 67, 86, 255};
		
		std::array<PositionColor, 6> const verticesCombo = {
			Vector3(-1.0f, -1.0f, 0.0f), colorLower.ToVector4(),// [0]
			Vector3(-1.0f,  1.0f, 0.0f), colorUpper.ToVector4(),// [1]
			Vector3( 1.0f,  1.0f, 0.0f), colorUpper.ToVector4(),// [2]
			Vector3( 1.0f,  1.0f, 0.0f), colorUpper.ToVector4(),// [2]
			Vector3( 1.0f, -1.0f, 0.0f), colorLower.ToVector4(),// [3]
			Vector3(-1.0f, -1.0f, 0.0f), colorLower.ToVector4(),// [0]
		};
		vertexBuffer = std::make_shared<VertexBuffer>(graphicsDevice,
			PositionColor::Declaration(), verticesCombo.data(), verticesCombo.size(), BufferUsage::Immutable);
	}
	{
		effectPass = assets->Load<EffectPass>("Effects/PrimitiveLineEffect");
		inputLayout = std::make_shared<InputLayout>(graphicsDevice, effectPass);
	}
}
//-----------------------------------------------------------------------
void GradientPlane::Draw()
{
	auto parameter = effectPass->Parameters("TransformMatrix");
	parameter->SetValue(Matrix4x4::Identity);

	graphicsContext->SetInputLayout(inputLayout);
	graphicsContext->SetVertexBuffer(vertexBuffer);
	effectPass->Apply();
	graphicsContext->Draw(PrimitiveTopology::TriangleList, vertexBuffer->VertexCount());
}
//-----------------------------------------------------------------------
}// namespace SceneEditor
}// namespace Pomdog
