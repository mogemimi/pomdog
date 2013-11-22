//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/GraphicsContext.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <utility>
#include "../RenderSystem/NativeGraphicsContext.hpp"

namespace Pomdog {

using Details::RenderSystem::NativeGraphicsContext;

//-----------------------------------------------------------------------
class GraphicsContext::Impl
{
public:
	std::unique_ptr<NativeGraphicsContext> nativeContext;
	
	explicit Impl(std::unique_ptr<NativeGraphicsContext> nativeContext)
		: nativeContext(std::move(nativeContext))
	{}
};
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
void GraphicsContext::Present()
{
	POMDOG_ASSERT(impl);
	POMDOG_ASSERT(impl->nativeContext);

	impl->nativeContext->Present();
}

}// namespace Pomdog
