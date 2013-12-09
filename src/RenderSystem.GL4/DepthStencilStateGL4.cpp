//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "DepthStencilStateGL4.hpp"
#include "OpenGLPrerequisites.hpp"
#include <Pomdog/Graphics/DepthStencilDescription.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Utility/detail/Tagged.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

using ComparisonFunctionGL4 = Tagged<GLenum, ComparisonFunction>;
using StencilOperationGL4 = Tagged<GLenum, StencilOperation>;

//-----------------------------------------------------------------------
static GLenum ToComparisonFunctionGL4NonTypesafe(ComparisonFunction const& comparison)
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
#ifdef _MSC_VER
	return GL_LEQUAL;
#endif
}
//-----------------------------------------------------------------------
static GLenum ToStencilOperationGL4NonTypesafe(StencilOperation const& operation)
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
#ifdef _MSC_VER
	return GL_KEEP;
#endif
}
//-----------------------------------------------------------------------
static ComparisonFunctionGL4 ToComparisonFunctionGL4(ComparisonFunction const& comparison)
{
	return ComparisonFunctionGL4{
		ToComparisonFunctionGL4NonTypesafe(comparison)
	};
}
//-----------------------------------------------------------------------
static StencilOperationGL4 ToStencilOperationGL4(StencilOperation const& operation)
{
	return StencilOperationGL4{
		ToStencilOperationGL4NonTypesafe(operation)
	};
}
//-----------------------------------------------------------------------
struct DepthStencilFaceOperationGL4 final
{
	ComparisonFunctionGL4 stencilFunction;
	StencilOperationGL4 stencilFail;
	StencilOperationGL4 stencilDepthBufferFail;
	StencilOperationGL4 stencilPass;

	DepthStencilFaceOperationGL4()
		: stencilFunction(GL_ALWAYS)
		, stencilDepthBufferFail(GL_KEEP)
		, stencilFail(GL_KEEP)
		, stencilPass(GL_KEEP)
	{}
};
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
	void ApplyStencilTest();

public:
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
	glDepthFunc(depthFunction.value);
}
//-----------------------------------------------------------------------
void DepthStencilStateGL4::Impl::ApplyStencilTest()
{
	if (!stencilEnable) {
		glDisable(GL_STENCIL_TEST);
		return;
	}

	glEnable(GL_STENCIL_TEST);

	// CounterClockwiseFace:
	glStencilFuncSeparate(GL_FRONT,
		counterClockwiseFace.stencilFunction.value,
		referenceStencil,
		stencilMask
	);
	glStencilOpSeparate(GL_FRONT,
		counterClockwiseFace.stencilFail.value,
		counterClockwiseFace.stencilDepthBufferFail.value,
		counterClockwiseFace.stencilPass.value
	);

	// ClockwiseFace:
	glStencilFuncSeparate(GL_BACK,
		clockwiseFace.stencilFunction.value,
		referenceStencil,
		stencilMask
	);
	glStencilOpSeparate(GL_BACK,
		clockwiseFace.stencilFail.value,
		clockwiseFace.stencilDepthBufferFail.value,
		clockwiseFace.stencilPass.value
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
	impl->ApplyStencilTest();
}

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
