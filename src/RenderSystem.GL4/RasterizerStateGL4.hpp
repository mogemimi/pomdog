//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_GL4_RASTERIZERSTATEGL4_CD96D51A_56D8_4EC5_BD1F_2FB19D26A089_HPP
#define POMDOG_SRC_GL4_RASTERIZERSTATEGL4_CD96D51A_56D8_4EC5_BD1F_2FB19D26A089_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "OpenGLPrerequisites.hpp"
#include "../RenderSystem/NativeRasterizerState.hpp"
#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>
#include <Pomdog/Utility/detail/Tagged.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

using FillModeGL4 = Tagged<GLenum, FillMode>;

class RasterizerStateGL4 final: public NativeRasterizerState
{
public:
	RasterizerStateGL4() = delete;
	
	explicit RasterizerStateGL4(RasterizerDescription const& description);
	
	~RasterizerStateGL4() = default;
	
	///@copydoc NativeRasterizerState
	void Apply() override;
	
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
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_GL4_RASTERIZERSTATEGL4_CD96D51A_56D8_4EC5_BD1F_2FB19D26A089_HPP)
