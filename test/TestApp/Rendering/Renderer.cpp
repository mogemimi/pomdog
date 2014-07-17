//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "Renderer.hpp"
#include "RenderQueue.hpp"
#include "RenderCommand.hpp"
#include "../Graphics/SpriteRenderer.hpp"
//#include "../Graphics/PolygonBatch.hpp"
#include "SpriteBatchRenderer.hpp"
#include "SpriteCommand.hpp"

namespace Pomdog {

using Details::Rendering::SpriteBatchRenderer;
using Details::Rendering::SpriteCommand;

//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - Renderer::Impl
#endif
//-----------------------------------------------------------------------
class Renderer::Impl {
public:
	Impl(std::shared_ptr<GraphicsContext> const& graphicsContext,
		std::shared_ptr<GraphicsDevice> const& graphicsDevice, AssetManager & assets);

	void Render(std::shared_ptr<GraphicsContext> const& graphicsContext);

	void Flush();

public:
	RenderQueue renderQueue;
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;
	std::unique_ptr<SpriteBatchRenderer> spriteBatch;
	//std::unique_ptr<PolygonBatch> polygonBatch;
	//std::unique_ptr<SpriteBatch> spriteBatch;
	
	std::uint32_t drawCallCount;
	
	enum BatchState: std::uint8_t {
		None,
		Sprite,
	};
	
	BatchState batchState;
};
//-----------------------------------------------------------------------
Renderer::Impl::Impl(std::shared_ptr<GraphicsContext> const& graphicsContext,
	std::shared_ptr<GraphicsDevice> const& graphicsDevice, AssetManager & assets)
	: viewMatrix{Matrix4x4::Identity}
	, projectionMatrix{Matrix4x4::Identity}
	, drawCallCount{0}
	, batchState{BatchState::None}
{
	spriteBatch = std::make_unique<SpriteBatchRenderer>(graphicsContext, graphicsDevice, assets);
	//polygonBatch = std::make_unique<PolygonBatch>(graphicsContext, graphicsDevice, *assets);
	//spriteRenderer->SetProjectionMatrix(Matrix4x4::CreateOrthographicLH(bounds.Width, bounds.Height, 1.0f, 100.0f));
}
//-----------------------------------------------------------------------
void Renderer::Impl::Render(std::shared_ptr<GraphicsContext> const& graphicsContext)
{
	drawCallCount = 0;
	
	auto viewProjection = viewMatrix * projectionMatrix;
	spriteBatch->SetProjectionMatrix(viewProjection);

	renderQueue.Sort();
	renderQueue.Enumerate([&](RenderCommand & command)
	{
		switch (command.CommandType()) {
		case RenderCommandType::Custom:
		case RenderCommandType::Batch: {
			Flush();
			POMDOG_ASSERT(batchState == BatchState::None);
			command.Execute(graphicsContext);
			++drawCallCount;
			break;
		}
		case RenderCommandType::Sprite: {
			if (batchState != BatchState::Sprite) {
				Flush();
				spriteBatch->Begin(Matrix4x4::Identity);
				batchState = BatchState::Sprite;
			}
			
			auto spriteCommand = static_cast<SpriteCommand*>(&command);
			spriteBatch->Draw(spriteCommand->texture, spriteCommand->transform,
				spriteCommand->textureRegion.Subrect, spriteCommand->color, spriteCommand->originPivot);

			break;
		}
		}
	});
	
	Flush();
	renderQueue.Clear();
	POMDOG_ASSERT(batchState == BatchState::None);
}
//-----------------------------------------------------------------------
void Renderer::Impl::Flush()
{
	switch (batchState) {
	case BatchState::Sprite: {
		POMDOG_ASSERT(spriteBatch);
		spriteBatch->End();
		drawCallCount += spriteBatch->DrawCallCount();
		break;
	}
	case BatchState::None:
		break;
	}

	batchState = BatchState::None;
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - Renderer
#endif
//-----------------------------------------------------------------------
Renderer::Renderer(std::shared_ptr<GraphicsContext> const& graphicsContext,
	std::shared_ptr<GraphicsDevice> const& graphicsDevice, AssetManager & assets)
	: impl(std::make_unique<Impl>(graphicsContext, graphicsDevice, assets))
{}
//-----------------------------------------------------------------------
Renderer::~Renderer() = default;
//-----------------------------------------------------------------------
void Renderer::Render(std::shared_ptr<GraphicsContext> const& graphicsContext)
{
	POMDOG_ASSERT(impl);
	impl->Render(graphicsContext);
}
//-----------------------------------------------------------------------
void Renderer::PushCommand(std::reference_wrapper<RenderCommand> && command)
{
	POMDOG_ASSERT(impl);
	impl->renderQueue.PushBack(std::move(command));
}
//-----------------------------------------------------------------------
void Renderer::ViewMatrix(Matrix4x4 const& viewMatrixIn)
{
	POMDOG_ASSERT(impl);
	impl->viewMatrix = viewMatrixIn;
}
//-----------------------------------------------------------------------
void Renderer::ProjectionMatrix(Matrix4x4 const& projectionMatrixIn)
{
	POMDOG_ASSERT(impl);
	impl->projectionMatrix = projectionMatrixIn;
}
//-----------------------------------------------------------------------
std::uint32_t Renderer::DrawCallCount() const
{
	POMDOG_ASSERT(impl);
	return impl->drawCallCount;
}
//-----------------------------------------------------------------------
}// namespace Pomdog
