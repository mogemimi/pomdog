//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "PrimitiveGrid.hpp"

namespace TestApp {
//-----------------------------------------------------------------------
PrimitiveGrid::PrimitiveGrid(std::shared_ptr<GameHost> const& gameHost,
	Color const& primaryColor, Color const& secondaryColor)
{
	auto graphicsDevice = gameHost->GraphicsDevice();
	auto assets = gameHost->AssetManager();

	{
		using PositionColor = CustomVertex<Vector2, Vector4>;
		
		std::uint32_t const gridCount = 30;
		
		std::vector<PositionColor> verticesCombo;
		verticesCombo.reserve((1 + gridCount*2) * 4);
		
		POMDOG_ASSERT(gridCount > 0);
		auto const lineLength = 1.0f * (gridCount);
		
		auto const primaryColorVector = primaryColor.ToVector4();
		auto const secondaryColorVector = secondaryColor.ToVector4();
		
		for (size_t i = 0; i < (1 + 2 * gridCount); ++i)
		{
			auto lineNumber = ((i + 1) / 2);
			auto offset = (1.0f * lineNumber) * ((i % 2 == 0) ? 1.0f: -1.0f);

			Vector4 color = (lineNumber % 10 == 0) ? primaryColorVector: secondaryColorVector;

			verticesCombo.push_back({Vector2{-lineLength, offset}, color});
			verticesCombo.push_back({Vector2{+lineLength, offset}, color});
			verticesCombo.push_back({Vector2{offset, -lineLength}, color});
			verticesCombo.push_back({Vector2{offset, +lineLength}, color});
		}

		vertexBuffer = std::make_shared<VertexBuffer>(graphicsDevice,
			PositionColor::Declaration(), verticesCombo.data(), verticesCombo.size(), BufferUsage::Immutable);
	}
	{
		effectPass = assets->Load<EffectPass>("PrimitiveGridEffect");
		inputLayout = std::make_shared<InputLayout>(graphicsDevice, effectPass);
	}
}
//-----------------------------------------------------------------------
void PrimitiveGrid::Draw(GraphicsContext & graphicsContext, Matrix4x4 const& transformMatrix)
{
	struct alignas(16) GridLayout
	{
		Matrix4x4 ViewMatrix;
	};
	
	constexpr float gridPixelSize = 96.0f;
	auto gridScaling = Matrix4x4::CreateScale({gridPixelSize, gridPixelSize, gridPixelSize});

	auto parameter = effectPass->Parameters("GridLayout");
	parameter->SetValue(GridLayout{Matrix4x4::Transpose(gridScaling * transformMatrix)});

	graphicsContext.SetInputLayout(inputLayout);
	graphicsContext.SetVertexBuffer(vertexBuffer);
	effectPass->Apply();
	graphicsContext.Draw(PrimitiveTopology::LineList);
}

}// namespace TestApp
