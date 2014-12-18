//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Renderer.hpp"
#include "RenderQueue.hpp"
#include "RenderCommand.hpp"
#include "Pomdog.Experimental/Rendering/RenderCommandProcessor.hpp"
#include <unordered_map>
#include <typeindex>

namespace Pomdog {
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - Renderer::Impl
#endif
//-----------------------------------------------------------------------
class Renderer::Impl {
public:
	Impl(std::shared_ptr<GraphicsContext> const& graphicsContext,
		std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	void AddProcessor(std::type_index const& index, std::unique_ptr<RenderCommandProcessor> && processor);

	void Render(GraphicsContext & graphicsContext);
	
	void Clear();

public:
	std::unordered_map<std::type_index, std::unique_ptr<RenderCommandProcessor>> processors;
	RenderQueue renderQueue;
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;
	std::uint32_t drawCallCount;
};
//-----------------------------------------------------------------------
Renderer::Impl::Impl(std::shared_ptr<GraphicsContext> const& graphicsContext,
	std::shared_ptr<GraphicsDevice> const& graphicsDevice)
	: viewMatrix{Matrix4x4::Identity}
	, projectionMatrix{Matrix4x4::Identity}
	, drawCallCount{0}
{
}
//-----------------------------------------------------------------------
void Renderer::Impl::AddProcessor(std::type_index const& index, std::unique_ptr<RenderCommandProcessor> && processor)
{
	POMDOG_ASSERT(processor);
	processors.emplace(index, std::move(processor));
}
//-----------------------------------------------------------------------
void Renderer::Impl::Render(GraphicsContext & graphicsContext)
{
	drawCallCount = 0;
	
	for (auto & iter: processors) {
		auto & processor = iter.second;
		POMDOG_ASSERT(processor);
		processor->SetViewProjection(viewMatrix, projectionMatrix);
	}

	auto prevIter = std::end(processors);

	renderQueue.Sort();
	renderQueue.Enumerate([&](RenderCommand & command)
	{
		auto iter = processors.find(command.TypeIndex());
		
		if (prevIter != iter)
		{
			if (prevIter != std::end(processors))
			{
				auto & processor = prevIter->second;
				
				POMDOG_ASSERT(processor);
				processor->End(graphicsContext);
				
				POMDOG_ASSERT(processor->DrawCallCount() >= 0);
				drawCallCount += processor->DrawCallCount();
			}
		
			if (iter != std::end(processors))
			{
				auto & processor = iter->second;
				
				POMDOG_ASSERT(processor);
				processor->Begin(graphicsContext);
			}
		
			prevIter = iter;
		}
		
		POMDOG_ASSERT(prevIter == iter);
		
		if (iter == std::end(processors)) {
			// warning
			return;
		}
		
		POMDOG_ASSERT(iter != std::end(processors));
		POMDOG_ASSERT(iter->second);
		
		auto & processor = iter->second;
		
		processor->Draw(graphicsContext, command);
	});
	
	if (std::end(processors) != prevIter)
	{
		POMDOG_ASSERT(prevIter->second);
		auto & processor = prevIter->second;
		processor->End(graphicsContext);
		
		POMDOG_ASSERT(processor->DrawCallCount() >= 0);
		drawCallCount += processor->DrawCallCount();
	}
}
//-----------------------------------------------------------------------
void Renderer::Impl::Clear()
{
	renderQueue.Clear();
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
void Renderer::Clear()
{
	POMDOG_ASSERT(impl);
	impl->Clear();
}
//-----------------------------------------------------------------------
void Renderer::AddProcessor(std::type_index const& index, std::unique_ptr<RenderCommandProcessor> && processor)
{
	POMDOG_ASSERT(impl);
	impl->AddProcessor(index, std::move(processor));
}
//-----------------------------------------------------------------------
}// namespace Pomdog
