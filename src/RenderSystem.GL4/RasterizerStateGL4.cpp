//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "RasterizerStateGL4.hpp"
#include "ErrorChecker.hpp"
#include <Pomdog/Graphics/RasterizerDescription.hpp>
#include <Pomdog/Utility/Assert.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {
//-----------------------------------------------------------------------
namespace {

static FillModeGL4 ToFillModeGL4(FillMode const& fillMode)
{
	switch (fillMode) {
	case FillMode::Solid: return FillModeGL4{ GL_FILL };
	case FillMode::WireFrame: return FillModeGL4{ GL_LINE };
	}
#ifdef _MSC_VER
	return FillModeGL4{ GL_FILL };
#endif
}

}// unnamed namespace
//-----------------------------------------------------------------------
RasterizerStateGL4::RasterizerStateGL4(RasterizerDescription const& description)
	: fillMode(ToFillModeGL4(description.FillMode))
	, cullMode(description.CullMode)
	, depthBias(static_cast<GLfloat>(description.DepthBias))
	, slopeScaledDepthBias(static_cast<GLfloat>(description.SlopeScaledDepthBias))
	, multisampleAntiAliasEnable(description.MultisampleEnable)
	, scissorTestEnable(description.ScissorTestEnable)
{
}
//-----------------------------------------------------------------------
void RasterizerStateGL4::Apply(NativeGraphicsContext &)
{
	#if defined(DEBUG) && !defined(NDEBUG)
	{
		GLint frontFace;
		glGetIntegerv(GL_FRONT_FACE, &frontFace);
		POMDOG_ASSERT(GL_CW == frontFace);
	}
	#endif

	// CullMode:
	switch (cullMode) {
	case CullMode::None:
		glDisable(GL_CULL_FACE);
		break;
	case CullMode::ClockwiseFace:
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		break;
	case CullMode::CounterClockwiseFace:
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		break;
	};

	// FillMode:
	glPolygonMode(GL_FRONT_AND_BACK, fillMode.value);
	
	#ifdef DEBUG
	ErrorChecker::CheckError("glPolygonMode", __FILE__, __LINE__);
	#endif
	
	// Scissor Test:
	if (scissorTestEnable) {
		glEnable(GL_SCISSOR_TEST);
	}
	else {
		glDisable(GL_SCISSOR_TEST);
	}
	
	// Depth bias:
	if (depthBias != 0.0f && slopeScaledDepthBias != 0.0f)
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(slopeScaledDepthBias, depthBias);
	}
	else {
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	
	// Multisample Anti-Aliasing:
	if (multisampleAntiAliasEnable) {
		glEnable(GL_MULTISAMPLE);
		
		#ifdef DEBUG
		ErrorChecker::CheckError("glEnable", __FILE__, __LINE__);
		#endif
	}
	else {
		glDisable(GL_MULTISAMPLE);
	}
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
