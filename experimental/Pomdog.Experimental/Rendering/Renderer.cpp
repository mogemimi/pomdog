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
#include "SpriteBatchRenderer.hpp"
#include "ParticleBatchCommand.hpp"
#include "SpriteCommand.hpp"
#include "PrimitiveCommand.hpp"
#include "Pomdog.Experimental/Graphics/SpriteRenderer.hpp"
#include "Pomdog.Experimental/Graphics/PolygonBatch.hpp"

namespace Pomdog {
namespace {

static Matrix3x2 CreateTransformMatrix(Particle const& particle)
{
	return Matrix3x2::CreateScale(particle.Size)
		* Matrix3x2::CreateRotation(particle.Rotation)
		* Matrix3x2::CreateTranslation(particle.Position);
}

}// unnamed namespace

using Details::Rendering::SpriteBatchRenderer;
using Details::Rendering::SpriteCommand;
using Details::Rendering::ParticleBatchCommand;
using Details::Rendering::PrimitiveCommand;

//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - Renderer::Impl
#endif
//-----------------------------------------------------------------------
class Renderer::Impl {
public:
	Impl(std::shared_ptr<GraphicsContext> const& graphicsContext,
		std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	void Render(GraphicsContext & graphicsContext);

	void Flush();

public:
	RenderQueue renderQueue;
	SpriteBatchRenderer spriteBatch;
	PolygonBatch primitiveBatch;
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;
	//std::unique_ptr<SpriteBatch> spriteBatch;
	
	std::uint32_t drawCallCount;
	
	enum BatchState: std::uint8_t {
		None,
		Primitive,
		Sprite,
	};
	
	BatchState batchState;
};
//-----------------------------------------------------------------------
Renderer::Impl::Impl(std::shared_ptr<GraphicsContext> const& graphicsContext,
	std::shared_ptr<GraphicsDevice> const& graphicsDevice)
	: spriteBatch{graphicsContext, graphicsDevice}
	, primitiveBatch{graphicsContext, graphicsDevice}
	, viewMatrix{Matrix4x4::Identity}
	, projectionMatrix{Matrix4x4::Identity}
	, drawCallCount{0}
	, batchState{BatchState::None}
{
}
//-----------------------------------------------------------------------
void Renderer::Impl::Render(GraphicsContext & graphicsContext)
{
	drawCallCount = 0;
	
	auto viewProjection = viewMatrix * projectionMatrix;
	spriteBatch.SetProjectionMatrix(viewProjection);

	renderQueue.Sort();
	renderQueue.Enumerate([&](RenderCommand & command)
	{
		switch (command.CommandType()) {
		case RenderCommandType::Custom: {
			Flush();
			POMDOG_ASSERT(batchState == BatchState::None);
			command.Execute(graphicsContext);
			++drawCallCount;
			break;
		}
		case RenderCommandType::ParticleBatch: {
			if (batchState != BatchState::Sprite) {
				Flush();
				spriteBatch.Begin(Matrix4x4::Identity);
				batchState = BatchState::Sprite;
			}
			
			POMDOG_ASSERT(batchState == BatchState::Sprite);
			auto & particleCommand = static_cast<ParticleBatchCommand &>(command);
			for (auto & particle: *particleCommand.particles)
			{
				auto transform = CreateTransformMatrix(particle);
				spriteBatch.Draw(particleCommand.texture, transform,
					particleCommand.textureRegion.Subrect, particle.Color, {0.5f, 0.5f});
			}
			break;
		}
		case RenderCommandType::Primitive: {
			if (batchState != BatchState::Primitive) {
				Flush();
				primitiveBatch.Begin(viewProjection);
				batchState = BatchState::Primitive;
			}
			
			POMDOG_ASSERT(batchState == BatchState::Primitive);
			auto & primitiveCommand = static_cast<PrimitiveCommand &>(command);
			primitiveBatch.DrawRectangle(primitiveCommand.transform,
				primitiveCommand.rectangle,
				primitiveCommand.leftBottomColor, primitiveCommand.rightBottomColor,
				primitiveCommand.rightTopColor, primitiveCommand.leftTopColor);
			break;
		}
		case RenderCommandType::Sprite: {
			if (batchState != BatchState::Sprite) {
				Flush();
				spriteBatch.Begin(Matrix4x4::Identity);
				batchState = BatchState::Sprite;
			}
			
			POMDOG_ASSERT(batchState == BatchState::Sprite);
			auto & spriteCommand = static_cast<SpriteCommand &>(command);
			spriteBatch.Draw(spriteCommand.texture, spriteCommand.transform,
				spriteCommand.textureRegion.Subrect, spriteCommand.color, spriteCommand.originPivot);

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
	case BatchState::Primitive: {
		primitiveBatch.End();
		
		///@todo Not implemented
		//drawCallCount += primitiveBatch.DrawCallCount();
		break;
	}
	case BatchState::Sprite: {
		spriteBatch.End();
		drawCallCount += spriteBatch.DrawCallCount();
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
	std::shared_ptr<GraphicsDevice> const& graphicsDevice)
	: impl(std::make_unique<Impl>(graphicsContext, graphicsDevice))
{}
//-----------------------------------------------------------------------
Renderer::~Renderer() = default;
//-----------------------------------------------------------------------
void Renderer::Render(GraphicsContext & graphicsContext)
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
