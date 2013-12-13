//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_NATIVEGRAPHICSCONTEXT_D292A72E_1944_423B_B25A_6A311774AC04_HPP
#define POMDOG_SRC_RENDERSYSTEM_NATIVEGRAPHICSCONTEXT_D292A72E_1944_423B_B25A_6A311774AC04_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Utility/Noncopyable.hpp>
#include <Pomdog/Math/detail/ForwardDeclarations.hpp>
#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class GraphicsCapabilities;

class NativeGraphicsContext: Noncopyable
{
public:
	virtual ~NativeGraphicsContext() = default;
	
	///@copydoc Pomdog::GraphicsContext
	virtual void Clear(Color const& color) = 0;
	
	///@copydoc Pomdog::GraphicsContext
	virtual void Clear(ClearOptions options, Color const& color, float depth, std::int32_t stencil) = 0;
	
	///@copydoc Pomdog::GraphicsContext
	virtual void Present() = 0;
	
	///@brief Retrieves the capabilities of a GraphicsContext
	virtual GraphicsCapabilities GetCapabilities() const = 0;

	///@copydoc Pomdog::GraphicsContext
	virtual void SetViewport(Viewport const& viewport) = 0;
	
	///@copydoc Pomdog::GraphicsContext
	virtual Rectangle GetScissorRectangle() const = 0;
	
	///@copydoc Pomdog::GraphicsContext
	virtual void SetScissorRectangle(Rectangle const& rectangle) = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVEGRAPHICSCONTEXT_D292A72E_1944_423B_B25A_6A311774AC04_HPP)
