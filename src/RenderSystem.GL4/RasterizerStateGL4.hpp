// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_RASTERIZERSTATEGL4_CD96D51A_56D8_4EC5_BD1F_2FB19D26A089_HPP
#define POMDOG_RASTERIZERSTATEGL4_CD96D51A_56D8_4EC5_BD1F_2FB19D26A089_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "OpenGLPrerequisites.hpp"
#include "../RenderSystem/NativeRasterizerState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Utility/detail/Tagged.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace GL4 {

using FillModeGL4 = Tagged<GLenum, FillMode>;

class RasterizerStateGL4 final: public NativeRasterizerState {
public:
	RasterizerStateGL4() = delete;

	explicit RasterizerStateGL4(RasterizerDescription const& description);

	///@copydoc NativeRasterizerState
	void Apply(NativeGraphicsContext & graphicsContext) override;

private:
	FillModeGL4 fillMode;
	CullMode cullMode;
	GLfloat depthBias;
	GLfloat slopeScaledDepthBias;
	bool multisampleAntiAliasEnable;
	bool scissorTestEnable;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_RASTERIZERSTATEGL4_CD96D51A_56D8_4EC5_BD1F_2FB19D26A089_HPP)
