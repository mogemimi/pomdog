//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "SpriteBatch.hpp"
#include <algorithm>
#include <vector>

namespace Pomdog {

#define POMDOG_SPRITEBATCH_COORDINATESYSTEM_DIRECT2D

//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - SpriteBatch::Impl
#endif
//-----------------------------------------------------------------------
class SpriteBatch::Impl {
private:
	static constexpr std::size_t MaxBatchSize = 2048;
	static constexpr std::size_t MinBatchSize = 128;
	
	static_assert(MaxBatchSize >= MinBatchSize, "");

	struct alignas(16) SpriteInfo {
		// {xy__} = position.xy
		// {__zw} = scale.xy
		Vector4 Translation;
		
		// {xy__} = xy
		// {__zw} = {width, height}
		Vector4 SourceRect;
		
		// {xy__} = originPivot.xy
		// {__z_} = rotation
		// {___w} = layerDepth
		Vector4 OriginRotationLayerDepth;
		
		// {rgb_} = color.rgb
		// {___a} = color.a
		Vector4 Color;
	};

private:
	std::shared_ptr<GraphicsContext> graphicsContext;
	
	std::vector<std::vector<SpriteInfo>> spriteQueues;
	std::vector<std::shared_ptr<Texture2D>> textures;

	std::shared_ptr<VertexBuffer> planeVertices;
	std::shared_ptr<IndexBuffer> planeIndices;
	std::shared_ptr<VertexBuffer> instanceVertices;
	
	std::shared_ptr<EffectPass> effectPass;
	std::shared_ptr<InputLayout> inputLayout;
	alignas(16) Matrix4x4 transposedTransformProjectionMatrix;
	
	SpriteSortMode sortMode;

public:
	Impl(std::shared_ptr<GraphicsContext> const& graphicsContext,
		std::shared_ptr<GraphicsDevice> const& graphicsDevice, AssetManager & assets);
	
	Impl(std::shared_ptr<GraphicsContext> const& graphicsContext,
		std::shared_ptr<GraphicsDevice> const& graphicsDevice, AssetManager & assets,
		std::shared_ptr<EffectPass> const& effectPass);
	
	void Begin(SpriteSortMode sortMode);
	
	void Begin(SpriteSortMode sortMode, Matrix4x4 const& transformMatrix);
	
	void Draw(std::shared_ptr<Texture2D> const& texture,
		Vector2 const& position, Color const& color,
		Radian<float> const& rotation, Vector2 const& originPivot, Vector2 const& scale, float layerDepth);
	
	void Draw(std::shared_ptr<Texture2D> const& texture,
		Vector2 const& position, Rectangle const& sourceRect, Color const& color,
		Radian<float> const& rotation, Vector2 const& originPivot, Vector2 const& scale, float layerDepth);
	
	void End();
	
private:
	void Flush();
	void DrawInstance(std::shared_ptr<Texture2D> const& texture, std::vector<SpriteInfo> const& sprites);
};
//-----------------------------------------------------------------------
SpriteBatch::Impl::Impl(std::shared_ptr<GraphicsContext> const& graphicsContextIn,
	std::shared_ptr<GraphicsDevice> const& graphicsDevice, AssetManager & assets)
	: Impl(graphicsContextIn, graphicsDevice, assets, assets.Load<EffectPass>("Effects/SpriteBatch"))
{}
//-----------------------------------------------------------------------
SpriteBatch::Impl::Impl(std::shared_ptr<GraphicsContext> const& graphicsContextIn,
	std::shared_ptr<GraphicsDevice> const& graphicsDevice, AssetManager & assets,
	std::shared_ptr<EffectPass> const& effectPassIn)
	: graphicsContext(graphicsContextIn)
	, transposedTransformProjectionMatrix(Matrix4x4::Identity)
	, effectPass(effectPassIn)
	, sortMode(SpriteSortMode::BackToFront)
{
	using PositionTextureCoord = CustomVertex<Vector4>;
	using SpriteInfoVertex = CustomVertex<Vector4, Vector4, Vector4, Vector4>;
	
	{
	
		std::array<PositionTextureCoord, 4> const verticesCombo = {
		#ifdef POMDOG_SPRITEBATCH_COORDINATESYSTEM_DIRECT2D
			Vector4(0.0f, 0.0f, 0.0f, 0.0f), // left-top
			Vector4(0.0f, 1.0f, 0.0f, 1.0f), // left-bottom
			Vector4(1.0f, 1.0f, 1.0f, 1.0f), // right-bottom
			Vector4(1.0f, 0.0f, 1.0f, 0.0f), // right-top
		#else
			Vector4(0.0f, 0.0f, 0.0f, 1.0f),
			Vector4(0.0f, 1.0f, 0.0f, 0.0f),
			Vector4(1.0f, 1.0f, 1.0f, 0.0f),
			Vector4(1.0f, 0.0f, 1.0f, 1.0f),
		#endif
		};
		planeVertices = std::make_shared<VertexBuffer>(graphicsDevice,
			verticesCombo.data(), verticesCombo.size(),
			PositionTextureCoord::Declaration().StrideBytes(), BufferUsage::Immutable);
	}
	{
		std::array<std::uint16_t, 6> const indices = {
		#ifdef POMDOG_SPRITEBATCH_COORDINATESYSTEM_DIRECT2D
			0, 2, 1,
			0, 3, 2
		#else
			0, 1, 2,
			2, 3, 0
		#endif
		};

		// Create index buffer
		planeIndices = std::make_shared<IndexBuffer>(graphicsDevice,
			IndexElementSize::SixteenBits, indices.data(), indices.size(), BufferUsage::Immutable);
	}
	{
		auto maxBatchSize = MaxBatchSize;
		instanceVertices = std::make_shared<VertexBuffer>(graphicsDevice,
			maxBatchSize, sizeof(SpriteInfo), BufferUsage::Dynamic);
	}
	{
		auto declartation = PositionTextureCoord::Declaration();

		POMDOG_ASSERT(effectPass);
		inputLayout = std::make_shared<InputLayout>(graphicsDevice, effectPass,
			std::initializer_list<VertexBufferBinding>{
				{declartation, 0, 0},
				{SpriteInfoVertex::Declaration(), declartation.StrideBytes(), 1}
			});
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
void SpriteBatch::Impl::Begin(SpriteSortMode sortModeIn)
{
	this->sortMode = sortModeIn;

	auto viewport = graphicsContext->Viewport();

	POMDOG_ASSERT(viewport.Width() > 0);
	POMDOG_ASSERT(viewport.Height() > 0);
	
#ifdef POMDOG_SPRITEBATCH_COORDINATESYSTEM_DIRECT2D
	transposedTransformProjectionMatrix = Matrix4x4::Transpose(
		Matrix4x4::CreateTranslation(Vector3(-viewport.Width() / 2, -viewport.Height() / 2, 1)) *
		Matrix4x4::CreateOrthographicLH(viewport.Width(), -viewport.Height(), 0.1f, 100.0f));
#else
	transposedTransformProjectionMatrix = Matrix4x4::Transpose(
		Matrix4x4::CreateOrthographicLH(viewport.Width(), viewport.Height(), 0.1f, 100.0f));
#endif

//	auto parameter = effectPass->Parameters("Matrices");
//	parameter->SetValue(transposedTransformProjectionMatrix);
}
//-----------------------------------------------------------------------
void SpriteBatch::Impl::Begin(SpriteSortMode sortModeIn, Matrix4x4 const& transformMatrix)
{
	this->sortMode = sortModeIn;

	auto viewport = graphicsContext->Viewport();

	POMDOG_ASSERT(viewport.Width() > 0);
	POMDOG_ASSERT(viewport.Height() > 0);
	
#ifdef POMDOG_SPRITEBATCH_COORDINATESYSTEM_DIRECT2D
	transposedTransformProjectionMatrix = Matrix4x4::Transpose(transformMatrix
		* Matrix4x4::CreateTranslation(Vector3(-viewport.Width() / 2, -viewport.Height() / 2, 1))
		* Matrix4x4::CreateOrthographicLH(viewport.Width(), -viewport.Height(), 0.1f, 100.0f));
#else
	transposedTransformProjectionMatrix = Matrix4x4::Transpose(transformMatrix
		* Matrix4x4::CreateOrthographicLH(viewport.Width(), viewport.Height(), 0.1f, 100.0f));
#endif

//	auto parameter = effectPass->Parameters("Matrices");
//	parameter->SetValue(transposedTransformProjectionMatrix);
}
//-----------------------------------------------------------------------
void SpriteBatch::Impl::End()
{
	Flush();
}
//-----------------------------------------------------------------------
void SpriteBatch::Impl::Flush()
{
	if (textures.empty()) {
		return;
	}
	
	for (std::size_t queuePos = 0; queuePos < textures.size(); ++queuePos)
	{
		POMDOG_ASSERT(!spriteQueues.empty());
		POMDOG_ASSERT(queuePos < spriteQueues.size());
		POMDOG_ASSERT(!spriteQueues[queuePos].empty());
		
		auto & sprites = spriteQueues[queuePos];
		
		switch (sortMode) {
		case SpriteSortMode::BackToFront:
			std::sort(std::begin(sprites), std::end(sprites), [](SpriteInfo const& a, SpriteInfo const& b) {
				return a.OriginRotationLayerDepth.W > b.OriginRotationLayerDepth.W;
			});
			break;
		case SpriteSortMode::FrontToBack:
			std::sort(std::begin(sprites), std::end(sprites), [](SpriteInfo const& a, SpriteInfo const& b) {
				return a.OriginRotationLayerDepth.W < b.OriginRotationLayerDepth.W;
			});
			break;
		case SpriteSortMode::Deferred:
			break;
		}

		DrawInstance(textures[queuePos], sprites);
		sprites.clear();
	}
	
	textures.clear();
	spriteQueues.clear();
}
//-----------------------------------------------------------------------
void SpriteBatch::Impl::DrawInstance(std::shared_ptr<Texture2D> const& texture, std::vector<SpriteInfo> const& sprites)
{
	POMDOG_ASSERT(texture);
	POMDOG_ASSERT(sprites.size() <= MaxBatchSize);
	
	{
		POMDOG_ASSERT(texture->Width() > 0);
		POMDOG_ASSERT(texture->Height() > 0);
		
		Vector2 inverseTextureSize {
			(texture->Width() > 0) ? (1.0f / static_cast<float>(texture->Width())): 0.0f,
			(texture->Height() > 0) ? (1.0f / static_cast<float>(texture->Height())): 0.0f,
		};
		
		struct alignas(16) SpriteBatchConstants {
			Matrix4x4 Transform;
			Vector2 InverseTextureSize;
		};
		
		alignas(16) SpriteBatchConstants info {
			transposedTransformProjectionMatrix,
			inverseTextureSize,
		};
		
		auto parameter = effectPass->Parameters("SpriteBatchConstants");
		parameter->SetValue(info);
	}

	POMDOG_ASSERT(sprites.size() <= MaxBatchSize);
	instanceVertices->SetData(sprites.data(), static_cast<std::uint32_t>(sprites.size()));

	graphicsContext->SetTexture(0, texture);
	graphicsContext->SetInputLayout(inputLayout);
	graphicsContext->SetVertexBuffers({planeVertices, instanceVertices});
	effectPass->Apply();
	graphicsContext->DrawIndexedInstanced(PrimitiveTopology::TriangleList,
		planeIndices, planeIndices->IndexCount(), static_cast<std::uint32_t>(sprites.size()));
}
//-----------------------------------------------------------------------
void SpriteBatch::Impl::Draw(std::shared_ptr<Texture2D> const& texture,
	Vector2 const& position, Color const& color,
	Radian<float> const& rotation, Vector2 const& originPivot, Vector2 const& scale, float layerDepth)
{
	if (scale.X == 0.0f || scale.Y == 0.0f) {
		return;
	}
	
	POMDOG_ASSERT(texture);

	if (spriteQueues.empty()
		|| (textures.back() != texture)
		|| (spriteQueues.back().size() >= MaxBatchSize)) {
		textures.push_back(texture);
		spriteQueues.push_back({});
		spriteQueues.back().reserve(MinBatchSize);
	}
	
	POMDOG_ASSERT(spriteQueues.size() <= MaxBatchSize);
	
	POMDOG_ASSERT(layerDepth >= 0.0f);
	POMDOG_ASSERT(layerDepth <= 1.0f);
	
	POMDOG_ASSERT(texture->Width() > 0);
	POMDOG_ASSERT(texture->Height() > 0);
	
	SpriteInfo info;
	info.Translation = Vector4(position.X, position.Y, scale.X, scale.Y);
	info.SourceRect = Vector4(0, 0, texture->Width(), texture->Height());
#ifdef POMDOG_SPRITEBATCH_COORDINATESYSTEM_DIRECT2D
	info.OriginRotationLayerDepth = Vector4(originPivot.X, originPivot.Y, -rotation.value, layerDepth);
#else
	info.OriginRotationLayerDepth = Vector4(originPivot.X, originPivot.Y, rotation.value, layerDepth);
#endif
	info.Color = color.ToVector4();
	
	spriteQueues.back().push_back(std::move(info));
	POMDOG_ASSERT(spriteQueues.back().size() <= MaxBatchSize);
}
//-----------------------------------------------------------------------
void SpriteBatch::Impl::Draw(std::shared_ptr<Texture2D> const& texture,
	Vector2 const& position, Rectangle const& sourceRect, Color const& color,
	Radian<float> const& rotation, Vector2 const& originPivot, Vector2 const& scale, float layerDepth)
{
	if (scale.X == 0.0f || scale.Y == 0.0f) {
		return;
	}
	
	POMDOG_ASSERT(texture);

	if (spriteQueues.empty()
		|| (textures.back() != texture)
		|| (spriteQueues.back().size() >= MaxBatchSize))
	{
		textures.push_back(texture);
		spriteQueues.push_back({});
		spriteQueues.back().reserve(MinBatchSize);
	}
	
	POMDOG_ASSERT(spriteQueues.size() <= MaxBatchSize);
	
	POMDOG_ASSERT(layerDepth >= 0.0f);
	POMDOG_ASSERT(layerDepth <= 1.0f);
	
	POMDOG_ASSERT(sourceRect.Width > 0);
	POMDOG_ASSERT(sourceRect.Height > 0);
	
	SpriteInfo info;
	info.Translation = Vector4(position.X, position.Y, scale.X, scale.Y);
	info.SourceRect = Vector4(sourceRect.X, sourceRect.Y, sourceRect.Width, sourceRect.Height);
#ifdef POMDOG_SPRITEBATCH_COORDINATESYSTEM_DIRECT2D
	info.OriginRotationLayerDepth = Vector4(originPivot.X, originPivot.Y, -rotation.value, layerDepth);
#else
	info.OriginRotationLayerDepth = Vector4(originPivot.X, originPivot.Y, rotation.value, layerDepth);
#endif
	info.Color = color.ToVector4();
	
	spriteQueues.back().push_back(std::move(info));
	POMDOG_ASSERT(spriteQueues.back().size() <= MaxBatchSize);
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - SpriteBatch
#endif
//-----------------------------------------------------------------------
SpriteBatch::SpriteBatch(std::shared_ptr<GraphicsContext> const& graphicsContext,
	std::shared_ptr<GraphicsDevice> const& graphicsDevice, AssetManager & assets)
	: impl(std::make_unique<Impl>(graphicsContext, graphicsDevice, assets))
{}
//-----------------------------------------------------------------------
SpriteBatch::SpriteBatch(std::shared_ptr<GraphicsContext> const& graphicsContext,
	std::shared_ptr<GraphicsDevice> const& graphicsDevice, AssetManager & assets,
	std::shared_ptr<EffectPass> const& effectPass)
	: impl(std::make_unique<Impl>(graphicsContext, graphicsDevice, assets, effectPass))
{}
//-----------------------------------------------------------------------
SpriteBatch::~SpriteBatch() = default;
//-----------------------------------------------------------------------
void SpriteBatch::Begin(SpriteSortMode sortMode)
{
	POMDOG_ASSERT(impl);
	impl->Begin(sortMode);
}
//-----------------------------------------------------------------------
void SpriteBatch::Begin(SpriteSortMode sortMode, Matrix4x4 const& transformMatrixIn)
{
	POMDOG_ASSERT(impl);
	impl->Begin(sortMode, transformMatrixIn);
}
//-----------------------------------------------------------------------
void SpriteBatch::End()
{
	POMDOG_ASSERT(impl);
	impl->End();
}
//-----------------------------------------------------------------------
void SpriteBatch::Draw(std::shared_ptr<Texture2D> const& texture,
	Rectangle const& sourceRect, Color const& color)
{
	POMDOG_ASSERT(impl);
	impl->Draw(texture, {0, 0}, sourceRect, color, 0, {0.5f, 0.5f}, {1.0f, 1.0f}, 0.0f);
}
//-----------------------------------------------------------------------
void SpriteBatch::Draw(std::shared_ptr<Texture2D> const& texture,
	Vector2 const& position, Color const& color)
{
	POMDOG_ASSERT(impl);
	impl->Draw(texture, {0, 0}, color, 0, {0.5f, 0.5f}, {1.0f, 1.0f}, 0.0f);
}
//-----------------------------------------------------------------------
void SpriteBatch::Draw(std::shared_ptr<Texture2D> const& texture,
	Vector2 const& position, Rectangle const& sourceRect, Color const& color)
{
	POMDOG_ASSERT(impl);
	impl->Draw(texture, position, sourceRect, color, 0, {0.5f, 0.5f}, {1.0f, 1.0f}, 0.0f);
}
//-----------------------------------------------------------------------
void SpriteBatch::Draw(std::shared_ptr<Texture2D> const& texture,
	Vector2 const& position, Rectangle const& sourceRect, Color const& color,
	Radian<float> const& rotation, Vector2 const& originPivot, float scale, float layerDepth)
{
	POMDOG_ASSERT(impl);
	impl->Draw(texture, position, sourceRect, color, rotation, originPivot, {scale, scale}, layerDepth);
}
//-----------------------------------------------------------------------
void SpriteBatch::Draw(std::shared_ptr<Texture2D> const& texture,
	Vector2 const& position, Rectangle const& sourceRect, Color const& color,
	Radian<float> const& rotation, Vector2 const& originPivot, Vector2 const& scale, float layerDepth)
{
	POMDOG_ASSERT(impl);
	impl->Draw(texture, position, sourceRect, color, rotation, originPivot, scale, layerDepth);
}
//-----------------------------------------------------------------------
}// namespace Pomdog
