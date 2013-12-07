//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "DepthStencilStateGL4.hpp"
#include "GL4Prerequisites.hpp"
#include <Pomdog/Graphics/DepthStencilDescription.hpp>
#include <Pomdog/Utility/Assert.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

struct DepthStencilFaceOperationGL4 final
{
	GLenum stencilFunction;
	GLenum stencilFail;
	GLenum stencilDepthBufferFail;
	GLenum stencilPass;

	DepthStencilFaceOperationGL4()
		: stencilFunction(GL_ALWAYS)
		, stencilDepthBufferFail(GL_KEEP)
		, stencilFail(GL_KEEP)
		, stencilPass(GL_KEEP)
	{}
};
//-----------------------------------------------------------------------
static GLenum ToComparisonFunctionGL4(ComparisonFunction const& comparison)
{
	switch (comparison)
	{
	case ComparisonFunction::Always: return GL_ALWAYS;
	case ComparisonFunction::Equal: return GL_EQUAL;
	case ComparisonFunction::Greater: return GL_GREATER;
	case ComparisonFunction::GreaterEqual: return GL_GEQUAL;
	case ComparisonFunction::Less: return GL_LESS;
	case ComparisonFunction::LessEqual: return GL_LEQUAL;
	case ComparisonFunction::Never: return GL_NEVER;
	case ComparisonFunction::NotEqual: return GL_NOTEQUAL;
	}
	return GL_LEQUAL;
}
//-----------------------------------------------------------------------
static GLenum ToStencilOperationGL4(StencilOperation const& operation)
{
	switch (operation)
	{
	case StencilOperation::Decrement: return GL_DECR_WRAP;
	case StencilOperation::DecrementSaturation: return GL_DECR; // Clamps to 0.
	case StencilOperation::Increment: return GL_INCR_WRAP;
	case StencilOperation::IncrementSaturation: return GL_INCR; // Clamps to the maximum representable unsigned value.
	case StencilOperation::Invert: return GL_INVERT;
	case StencilOperation::Keep: return GL_KEEP;
	case StencilOperation::Replace: return GL_REPLACE;
	case StencilOperation::Zero: return GL_ZERO;
	}
	return GL_KEEP;
}
//-----------------------------------------------------------------------
static void ToDepthStencilFaceOperationGL4(DepthStencilOperation const& face, DepthStencilFaceOperationGL4 & result)
{
	result.stencilFunction = ToComparisonFunctionGL4(face.StencilFunction);
	result.stencilDepthBufferFail = ToStencilOperationGL4(face.StencilDepthBufferFail);
	result.stencilFail = ToStencilOperationGL4(face.StencilFail);
	result.stencilPass = ToStencilOperationGL4(face.StencilPass);
}
//-----------------------------------------------------------------------
class DepthStencilStateGL4::Impl final
{
public:
	explicit Impl(DepthStencilDescription const& description);

	Impl(Impl const&) = delete;
	Impl& operator = (Impl const&) = delete;

	void ApplyDepthTest();
	void ApplyDepthStencil();

public:
	DepthStencilFaceOperationGL4 clockwiseFace;
	DepthStencilFaceOperationGL4 counterClockwiseFace;
	GLenum const depthFunction;
	
	GLint const referenceStencil;
	GLuint const stencilMask;
	GLuint const stencilWriteMask;
	bool const stencilEnable;
	bool const depthBufferEnable;
	GLboolean const depthBufferWriteEnable;
};
//-----------------------------------------------------------------------
DepthStencilStateGL4::Impl::Impl(DepthStencilDescription const& description)
	: depthFunction(ToComparisonFunctionGL4(description.DepthBufferFunction))
	, referenceStencil(static_cast<GLint>(description.ReferenceStencil))
	, stencilMask(static_cast<GLuint>(description.StencilMask))
	, stencilWriteMask(static_cast<GLuint>(description.StencilWriteMask))
	, stencilEnable(description.StencilEnable)
	, depthBufferEnable(description.DepthBufferEnable)
	, depthBufferWriteEnable(description.DepthBufferWriteEnable ? GL_TRUE: GL_FALSE)
{
	static_assert(std::is_same<GLuint, std::uint32_t>::value, "");
	static_assert(std::is_same<GLint, std::int32_t>::value, "");

	ToDepthStencilFaceOperationGL4(description.ClockwiseFace, clockwiseFace);
	ToDepthStencilFaceOperationGL4(description.CounterClockwiseFace, counterClockwiseFace);
}
//-----------------------------------------------------------------------
void DepthStencilStateGL4::Impl::ApplyDepthTest()
{
	if (!depthBufferEnable) {
		///@note
		/// http://www.opengl.org/sdk/docs/man/xhtml/glEnable.xml
		///
		/// Quote:
		///
		///  GL_DEPTH_TEST
		///  If enabled, do depth comparisons and update the depth buffer.
		///  Note that even if the depth buffer exists and the depth mask is non-zero,
		///  the depth buffer is not updated if the depth test is disabled.
		glDisable(GL_DEPTH_TEST);
		return;
	}

	glEnable(GL_DEPTH_TEST);

	// depth buffer write
	glDepthMask(depthBufferWriteEnable);

	// depth function
	glDepthFunc(depthFunction);
}
//-----------------------------------------------------------------------
void DepthStencilStateGL4::Impl::ApplyDepthStencil()
{
	if (!stencilEnable) {
		glDisable(GL_STENCIL_TEST);
		return;
	}

	glEnable(GL_STENCIL_TEST);

	glStencilFuncSeparate(GL_FRONT, counterClockwiseFace.stencilFunction, referenceStencil, stencilMask);
	glStencilOpSeparate(GL_FRONT,
		counterClockwiseFace.stencilFail,
		counterClockwiseFace.stencilDepthBufferFail,
		counterClockwiseFace.stencilPass
	);

	glStencilFuncSeparate(GL_BACK, clockwiseFace.stencilFunction, referenceStencil, stencilMask);
	glStencilOpSeparate(GL_BACK,
		clockwiseFace.stencilFail,
		clockwiseFace.stencilDepthBufferFail,
		clockwiseFace.stencilPass
	);
	
	glStencilMask(stencilWriteMask);
}
//-----------------------------------------------------------------------
DepthStencilStateGL4::DepthStencilStateGL4(DepthStencilDescription const& description)
	: impl(new Impl(description))
{}
//-----------------------------------------------------------------------
DepthStencilStateGL4::~DepthStencilStateGL4()
{}
//-----------------------------------------------------------------------
void DepthStencilStateGL4::Apply()
{
	POMDOG_ASSERT(impl);
	impl->ApplyDepthTest();
	impl->ApplyDepthStencil();
}

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
