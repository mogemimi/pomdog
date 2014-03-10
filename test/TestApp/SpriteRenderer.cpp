//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "SpriteRenderer.hpp"

namespace TestApp {
//-----------------------------------------------------------------------
SpriteRenderer::SpriteRenderer(std::shared_ptr<GameHost> const& gameHost)
	: graphicsContext(gameHost->GraphicsContext())
{
	auto graphicsDevice = gameHost->GraphicsDevice();
	auto assets = gameHost->AssetManager();

	{
		using PositionTextureCoord = CustomVertex<Vector4>;
		
		std::array<PositionTextureCoord, 4> const verticesCombo = {
			Vector4(0.0f, 0.0f, 0.0f, 1.0f),
			Vector4(0.0f, 1.0f, 0.0f, 0.0f),
			Vector4(1.0f, 1.0f, 1.0f, 0.0f),
			Vector4(1.0f, 0.0f, 1.0f, 1.0f),
		};
		vertexBuffer = std::make_shared<ImmutableVertexBuffer>(graphicsDevice,
			PositionTextureCoord::Declaration(), verticesCombo.data(), verticesCombo.size());
	}
	{
		effectPass = assets->Load<EffectPass>("Content/SpriteEffect");
		inputLayout = std::make_shared<InputLayout>(graphicsDevice, effectPass);
	}
	{
		std::array<std::uint16_t, 6> const indices = {
			0, 1, 2,
			2, 3, 0
		};

		// Create index buffer
		indexBuffer = std::make_shared<ImmutableIndexBuffer>(graphicsDevice,
			IndexElementSize::SixteenBits, indices.data(), indices.size());
	}
#ifdef DEBUG
	{
		for (auto & parameter: effectPass->Parameters()) {
			Log::Stream() << "EffectParameter: " << parameter.first;
		}
		
		auto effectReflection = std::make_shared<EffectReflection>(graphicsDevice, effectPass);
	
		auto stream = Log::Stream();
		for (auto & description: effectReflection->GetConstantBuffers()) {
			stream << "-----------------------" << "\n";
			stream << "     Name: " << description.Name << "\n";
			stream << " ByteSize: " << description.ByteSize << "\n";
			stream << "Variables: " << description.Variables.size() << "\n";
		}
	}
#endif
}
//-----------------------------------------------------------------------
void SpriteRenderer::Draw(std::shared_ptr<Texture2D> const& texture,
	Vector2 const& position, Vector2 const& scale, Radian<float> const& rotation, Rectangle const& sourceRect, Vector2 const& originPivot, float layerDepth)
{
	if (scale.x == 0.0f || scale.y == 0.0f) {
		return;
	}

	{
		using SpriteBatchInfo = std::array<Vector4, 4>;// (mat4x3(mat3x3(Projection2D)), vec4(InverseTexturePixelWidth.xy, 0, 0))
		
		POMDOG_ASSERT(texture->Width() > 0);
		POMDOG_ASSERT(texture->Height() > 0);
		
		Vector2 inverseTexturePixelWidth {
			(texture->Width() > 0) ? (1.0f / static_cast<float>(texture->Width())): 0.0f,
			(texture->Height() > 0) ? (1.0f / static_cast<float>(texture->Height())): 0.0f,
		};

		auto viewport = graphicsContext->GetViewport();
		
		POMDOG_ASSERT(viewport.Width() > 0.0f);
		POMDOG_ASSERT(viewport.Height() > 0.0f);
		
		auto scaleX = (viewport.Width() > 0.0f) ? (2.0f/viewport.Width()): 0.0f;
		auto scaleY = (viewport.Height() > 0.0f) ? (2.0f/viewport.Height()): 0.0f;
		
		constexpr float None = 0.0f;
		
		alignas(16) SpriteBatchInfo info = {
			Vector4(scaleX, 0.0f, 0.0f, None),
			Vector4(0.0f, scaleY, 0.0f, None),
			Vector4(0.0f, 0.0f, 1.0f, None),
			Vector4(inverseTexturePixelWidth.x, inverseTexturePixelWidth.y, None, None),
		};
		
		auto parameter = effectPass->Parameters("SpriteBatchInfo");
		parameter->SetValue(info);
	}
	{
		struct alignas(16) SpriteInfo
		{
			Vector4 Translation;				// (position.xy, scale.xy)
			Vector4 SourceRect;					// (x, y, width, height)
			Vector4 OriginRotationLayerDepth;	// (originPivot.xy, rotation, layerDepth)
		};
		
		POMDOG_ASSERT(layerDepth >= 0.0f);
		POMDOG_ASSERT(layerDepth <= 1.0f);
		
		POMDOG_ASSERT(sourceRect.width > 0);
		POMDOG_ASSERT(sourceRect.height > 0);
		
		SpriteInfo info;
		info.Translation = Vector4(position.x, position.y, scale.x, scale.y);
		info.SourceRect = Vector4(sourceRect.x, sourceRect.y, sourceRect.width, sourceRect.height);
		info.OriginRotationLayerDepth = Vector4(originPivot.x, originPivot.y, rotation.value, layerDepth);
		
		auto parameter = effectPass->Parameters("SpriteInfo");
		parameter->SetValue(info);
	}
	
	POMDOG_ASSERT(texture);

	graphicsContext->SetTexture(0, texture);
	graphicsContext->SetInputLayout(inputLayout);
	graphicsContext->SetVertexBuffer(vertexBuffer);
	effectPass->Apply();
	graphicsContext->DrawIndexed(PrimitiveTopology::TriangleList, indexBuffer, indexBuffer->IndexCount());
}
//-----------------------------------------------------------------------
}// namespace TestApp
