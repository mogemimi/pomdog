// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_DEPTHSTENCILSTATEGL4_B1C75682_46D9_4523_AE37_B5ED393AC50E_HPP
#define POMDOG_DEPTHSTENCILSTATEGL4_B1C75682_46D9_4523_AE37_B5ED393AC50E_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "OpenGLPrerequisites.hpp"
#include "../RenderSystem/NativeDepthStencilState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Utility/detail/Tagged.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace GL4 {

using ComparisonFunctionGL4 = Tagged<GLenum, ComparisonFunction>;
using StencilOperationGL4 = Tagged<GLenum, StencilOperation>;

struct DepthStencilFaceOperationGL4 final {
	ComparisonFunctionGL4 stencilFunction = GL_ALWAYS;
	StencilOperationGL4 stencilFail = GL_KEEP;
	StencilOperationGL4 stencilDepthBufferFail = GL_KEEP;
	StencilOperationGL4 stencilPass = GL_KEEP;
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
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_DEPTHSTENCILSTATEGL4_B1C75682_46D9_4523_AE37_B5ED393AC50E_HPP)
