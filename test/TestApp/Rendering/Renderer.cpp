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
#include "SpriteCommand.hpp"

namespace Pomdog {
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
	std::unique_ptr<SpriteRenderer> spriteRenderer;
	//std::unique_ptr<PolygonBatch> polygonBatch;
	//std::unique_ptr<SpriteBatch> spriteBatch;
};
//-----------------------------------------------------------------------
Renderer::Impl::Impl(std::shared_ptr<GraphicsContext> const& graphicsContext,
	std::shared_ptr<GraphicsDevice> const& graphicsDevice, AssetManager & assets)
{
	viewMatrix = Matrix4x4::Identity;
	projectionMatrix = Matrix4x4::Identity;
	spriteRenderer = std::make_unique<SpriteRenderer>(graphicsContext, graphicsDevice, assets);
	//spriteBatch = std::make_unique<SpriteBatch>(graphicsContext, graphicsDevice, *assets);
	//polygonBatch = std::make_unique<PolygonBatch>(graphicsContext, graphicsDevice, *assets);
	//spriteRenderer->SetProjectionMatrix(Matrix4x4::CreateOrthographicLH(bounds.Width, bounds.Height, 1.0f, 100.0f));
}
//-----------------------------------------------------------------------
void Renderer::Impl::Render(std::shared_ptr<GraphicsContext> const& graphicsContext)
{
	auto viewProjection = viewMatrix * projectionMatrix;
	spriteRenderer->SetProjectionMatrix(viewProjection);

	renderQueue.Sort();
	renderQueue.Enumerate([&](RenderCommand & command)
	{
		switch (command.CommandType()) {
		case RenderCommandType::Custom:
		case RenderCommandType::Batch: {
			Flush();
			command.Execute(graphicsContext);
			break;
		}
		case RenderCommandType::Sprite: {
			Flush();
			spriteRenderer->Begin(SpriteSortMode::BackToFront);

			using Details::Rendering::SpriteCommand;
			auto spriteCommand = static_cast<SpriteCommand*>(&command);
			spriteRenderer->Draw(spriteCommand->texture, spriteCommand->transform,
				Vector2::Zero, spriteCommand->textureRegion.Subrect, spriteCommand->color,
				0.0f, spriteCommand->origin, {1.0f, 1.0f}, 0.0f);
				
			spriteRenderer->End();
			break;
		}
		}
	});
	
	Flush();
	renderQueue.Clear();
}
//-----------------------------------------------------------------------
void Renderer::Impl::Flush()
{
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
}// namespace Pomdog
