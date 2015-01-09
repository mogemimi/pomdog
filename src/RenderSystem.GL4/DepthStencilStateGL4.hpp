//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SRC_GL4_DEPTHSTENCILSTATEGL4_B1C75682_46D9_4523_AE37_B5ED393AC50E_HPP
#define POMDOG_SRC_GL4_DEPTHSTENCILSTATEGL4_B1C75682_46D9_4523_AE37_B5ED393AC50E_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "OpenGLPrerequisites.hpp"
#include "../RenderSystem/NativeDepthStencilState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Utility/detail/Tagged.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

using ComparisonFunctionGL4 = Tagged<GLenum, ComparisonFunction>;
using StencilOperationGL4 = Tagged<GLenum, StencilOperation>;

struct DepthStencilFaceOperationGL4 final {
	ComparisonFunctionGL4 stencilFunction;
	StencilOperationGL4 stencilFail;
	StencilOperationGL4 stencilDepthBufferFail;
	StencilOperationGL4 stencilPass;

	DepthStencilFaceOperationGL4()
		: stencilFunction(GL_ALWAYS)
		, stencilFail(GL_KEEP)
		, stencilDepthBufferFail(GL_KEEP)
		, stencilPass(GL_KEEP)
	{}
};

class DepthStencilStateGL4 final: public NativeDepthStencilState {
public:
	DepthStencilStateGL4() = delete;
	
	explicit DepthStencilStateGL4(DepthStencilDescription const& description);
	
	///@copydoc NativeDepthStencilState
	void Apply(NativeGraphicsContext & graphicsContext) override;
	
private:
	void ApplyDepthTest();
	void ApplyStencilTest();
	
private:
	DepthStencilFaceOperationGL4 clockwiseFace;
	DepthStencilFaceOperationGL4 counterClockwiseFace;
	ComparisonFunctionGL4 const depthFunction;
	
	GLint const referenceStencil;
	GLuint const stencilMask;
	GLuint const stencilWriteMask;
	GLboolean const depthBufferWriteEnable;
	bool const stencilEnable;
	bool const depthBufferEnable;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_GL4_DEPTHSTENCILSTATEGL4_B1C75682_46D9_4523_AE37_B5ED393AC50E_HPP)
