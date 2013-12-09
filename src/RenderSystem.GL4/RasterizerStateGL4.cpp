//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "RasterizerStateGL4.hpp"
#include "OpenGLPrerequisites.hpp"
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Utility/detail/Tagged.hpp>
#include <Pomdog/Graphics/RasterizerDescription.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

using FillModeGL4 = Tagged<GLenum, FillMode>;

//-----------------------------------------------------------------------
static FillModeGL4 ToFillModeGL4(FillMode const& fillMode)
{
	switch (fillMode)
	{
	case FillMode::Solid: return FillModeGL4{ GL_FILL };
	case FillMode::WireFrame: return FillModeGL4{ GL_LINE };
	}
#ifdef _MSC_VER
	return FillModeGL4{ GL_FILL };
#endif
}
//-----------------------------------------------------------------------
class RasterizerStateGL4::Impl final
{
public:
	Impl() = delete;
	
	explicit Impl(RasterizerDescription const& description);
	
	void Apply();
	
private:
	FillModeGL4 fillMode;
	CullMode cullMode;
	GLfloat depthBias;
	GLfloat slopeScaledDepthBias;
	bool multisampleAntiAliasEnable;
	bool scissorTestEnable;
};
//-----------------------------------------------------------------------
RasterizerStateGL4::Impl::Impl(RasterizerDescription const& description)
	: fillMode(ToFillModeGL4(description.FillMode))
	, cullMode(description.CullMode)
	, depthBias(static_cast<GLfloat>(description.DepthBias))
	, slopeScaledDepthBias(static_cast<GLfloat>(description.SlopeScaledDepthBias))
	, multisampleAntiAliasEnable(description.MultisampleEnable)
	, scissorTestEnable(description.ScissorTestEnable)
{}
//-----------------------------------------------------------------------
void RasterizerStateGL4::Impl::Apply()
{
	// CullMode:
	switch (cullMode) {
	case CullMode::None:
		glDisable(GL_CULL_FACE);
		break;
	case CullMode::ClockwiseFace:
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		break;
	case CullMode::CounterClockwiseFace:
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		break;
	};

	// FillMode:
	glPolygonMode(GL_FRONT_AND_BACK, fillMode.value);
	
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
	}
	else {
		glDisable(GL_MULTISAMPLE);
	}
}
//-----------------------------------------------------------------------
RasterizerStateGL4::RasterizerStateGL4(RasterizerDescription const& description)
	: impl(new Impl(description))
{
}
//-----------------------------------------------------------------------
RasterizerStateGL4::~RasterizerStateGL4()
{
}
//-----------------------------------------------------------------------
void RasterizerStateGL4::Apply()
{
	POMDOG_ASSERT(impl);
	impl->Apply();
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
