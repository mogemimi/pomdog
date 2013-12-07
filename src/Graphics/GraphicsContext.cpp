//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/GraphicsContext.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Graphics/Viewport.hpp>
#include <utility>
#include "../RenderSystem/NativeGraphicsContext.hpp"

namespace Pomdog {

using Details::RenderSystem::NativeGraphicsContext;

//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark -
#pragma mark GraphicsContext::Impl class
#endif
//-----------------------------------------------------------------------
class GraphicsContext::Impl final
{
public:
	Impl() = delete;
	Impl(Impl const&) = delete;
	Impl(Impl &&) = default;
	
	explicit Impl(std::unique_ptr<NativeGraphicsContext> nativeContext);
	
public:
	std::unique_ptr<NativeGraphicsContext> nativeContext;
	Viewport viewport;
};
//-----------------------------------------------------------------------
GraphicsContext::Impl::Impl(std::unique_ptr<NativeGraphicsContext> nativeContext)
	: nativeContext(std::move(nativeContext))
{}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark -
#pragma mark GraphicsContext class
#endif
//-----------------------------------------------------------------------
GraphicsContext::GraphicsContext(std::unique_ptr<NativeGraphicsContext> nativeContext)
	: impl(new Impl(std::move(nativeContext)))
{}
//-----------------------------------------------------------------------
GraphicsContext::~GraphicsContext()
{}
//-----------------------------------------------------------------------
void GraphicsContext::Clear(Color const& color)
{
	POMDOG_ASSERT(impl);
	POMDOG_ASSERT(impl->nativeContext);

	impl->nativeContext->Clear(color);
}
//-----------------------------------------------------------------------
void GraphicsContext::Clear(ClearOptions options, Color const& color, float depth, std::int32_t stencil)
{
	POMDOG_ASSERT(impl);
	POMDOG_ASSERT(impl->nativeContext);

	impl->nativeContext->Clear(options, color, depth, stencil);
}
//-----------------------------------------------------------------------
void GraphicsContext::Present()
{
	POMDOG_ASSERT(impl);
	POMDOG_ASSERT(impl->nativeContext);

	impl->nativeContext->Present();
}
//-----------------------------------------------------------------------
Viewport const& GraphicsContext::GetViewport() const
{
	POMDOG_ASSERT(impl);
	POMDOG_ASSERT(impl->nativeContext);

	return impl->viewport;
}
//-----------------------------------------------------------------------
void GraphicsContext::SetViewport(Viewport const& viewport)
{
	POMDOG_ASSERT(impl);
	POMDOG_ASSERT(impl->nativeContext);
	POMDOG_ASSERT(viewport.GetWidth() > 0);
	POMDOG_ASSERT(viewport.GetHeight() > 0);

	impl->viewport = viewport;
	impl->nativeContext->SetViewport(viewport);
}

}// namespace Pomdog
