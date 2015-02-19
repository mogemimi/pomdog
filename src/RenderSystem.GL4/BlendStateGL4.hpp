//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_BLENDSTATEGL4_5A1F407F_670E_455C_BDEE_57E1472471F4_HPP
#define POMDOG_BLENDSTATEGL4_5A1F407F_670E_455C_BDEE_57E1472471F4_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "OpenGLPrerequisites.hpp"
#include "../RenderSystem/NativeBlendState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Utility/detail/Tagged.hpp"
#include "Pomdog/Math/Color.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

using BlendGL4 = Tagged<GLenum, Blend>;
using BlendFunctionGL4 = Tagged<GLenum, BlendFunction>;

class BlendStateGL4 final: public NativeBlendState {
public:
	BlendStateGL4() = delete;

	explicit BlendStateGL4(BlendDescription const& description);

	///@copydoc NativeBlendState
	void Apply(NativeGraphicsContext & graphicsContext) override;

private:
	Color blendColor;

	BlendFunctionGL4 const alphaFunction;
	BlendGL4 const alphaSource;
	BlendGL4 const alphaDestination;

	BlendFunctionGL4 const colorFunction;
	BlendGL4 const colorSource;
	BlendGL4 const colorDestination;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_BLENDSTATEGL4_5A1F407F_670E_455C_BDEE_57E1472471F4_HPP)
