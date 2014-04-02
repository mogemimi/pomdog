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
PrimitiveGrid::PrimitiveGrid(std::shared_ptr<GameHost> const& gameHost)
{
	auto graphicsDevice = gameHost->GraphicsDevice();
	auto assets = gameHost->AssetManager();

	{
		using Position = CustomVertex<Vector2>;
		
		size_t const gridCount = 12;
		
		std::vector<Position> verticesCombo;
		verticesCombo.reserve((1 + gridCount*2) * 4);
		
		POMDOG_ASSERT(gridCount > 0);
		auto const lineLength = 1.0f * (gridCount);
		
		for (size_t i = 0; i < (1 + gridCount*2); ++i)
		{
			auto offset = (1.0f * ((i + 1) / 2)) * ((i % 2 == 0) ? 1.0f: -1.0f);

			verticesCombo.push_back({Vector2(-lineLength, offset)});
			verticesCombo.push_back({Vector2(+lineLength, offset)});
			verticesCombo.push_back({Vector2(offset, -lineLength)});
			verticesCombo.push_back({Vector2(offset, +lineLength)});
		}

		vertexBuffer = std::make_shared<ImmutableVertexBuffer>(graphicsDevice,
			Position::Declaration(), verticesCombo.data(), verticesCombo.size());
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
		Vector4 LineColor;
	};
	
	constexpr float gridPixelSize = 64.0f;
	auto gridScaling = Matrix4x4::CreateScale({gridPixelSize, gridPixelSize, gridPixelSize});

	auto parameter = effectPass->Parameters("GridLayout");
	parameter->SetValue(GridLayout{ Matrix4x4::Transpose(gridScaling * transformMatrix), Vector4{1.0f, 1.0f, 1.0f, 1.0f}});

	graphicsContext.SetInputLayout(inputLayout);
	graphicsContext.SetVertexBuffer(vertexBuffer);
	effectPass->Apply();
	graphicsContext.Draw(PrimitiveTopology::LineList);
}

}// namespace TestApp
