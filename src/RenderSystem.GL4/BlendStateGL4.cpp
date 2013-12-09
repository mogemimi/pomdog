//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "BlendStateGL4.hpp"
#include "OpenGLPrerequisites.hpp"
#include <Pomdog/Graphics/BlendDescription.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Utility/detail/Tagged.hpp>
#include <utility>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

using BlendGL4 = Tagged<GLenum, Blend>;
using BlendFunctionGL4 = Tagged<GLenum, BlendFunction>;

//-----------------------------------------------------------------------
static GLenum ToBlendGL4NonTypesafe(Blend blend)
{
	switch (blend)
	{
	case Blend::Zero: return GL_ZERO;
	case Blend::One: return GL_ONE;
	case Blend::SourceColor: return GL_SRC_COLOR;
	case Blend::InverseSourceColor: return GL_ONE_MINUS_SRC_COLOR;
	case Blend::SourceAlpha: return GL_SRC_ALPHA;
	case Blend::InverseSourceAlpha: return GL_ONE_MINUS_SRC_ALPHA;
	case Blend::DestinationAlpha: return GL_DST_ALPHA;
	case Blend::InverseDestinationAlpha: return GL_ONE_MINUS_DST_ALPHA;
	case Blend::DestinationColor: return GL_DST_COLOR;
	case Blend::InverseDestinationColor: return GL_ONE_MINUS_DST_COLOR;
	case Blend::SourceAlphaSaturation: return GL_SRC_ALPHA_SATURATE;
	case Blend::BlendFactor: return GL_CONSTANT_COLOR;
	case Blend::InvereseBlendFactor: return GL_ONE_MINUS_CONSTANT_COLOR;
	}
#ifdef _MSC_VER
	return GL_ZERO;
#endif
}
//-----------------------------------------------------------------------
static GLenum ToBlendFunctionGL4NonTypesafe(BlendFunction func)
{
	switch (func)
	{
	case BlendFunction::Add: return GL_FUNC_ADD;
	case BlendFunction::Subtract: return GL_FUNC_SUBTRACT;
	case BlendFunction::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
	case BlendFunction::Min: return GL_MIN;
	case BlendFunction::Max: return GL_MAX;
	}
#ifdef _MSC_VER
	return GL_FUNC_ADD;
#endif
}
//-----------------------------------------------------------------------
static BlendGL4 ToBlendGL4(Blend blend)
{
	return std::move(BlendGL4{
		ToBlendGL4NonTypesafe(blend)
	});
}
//-----------------------------------------------------------------------
static BlendFunctionGL4 ToBlendFunctionGL4(BlendFunction func)
{
	return std::move(BlendFunctionGL4{
		ToBlendFunctionGL4NonTypesafe(func)
	});
}
//-----------------------------------------------------------------------
class BlendStateGL4::Impl final
{
public:
	Impl() = delete;
	
	explicit Impl(BlendDescription const& description);

	void Apply();
	
private:
	Color blendColor;

	BlendFunctionGL4 const alphaFunction;
	BlendGL4 const alphaSource;
	BlendGL4 const alphaDestination;

	BlendFunctionGL4 const colorFunction;
	BlendGL4 const colorSource;
	BlendGL4 const colorDestination;
};
//-----------------------------------------------------------------------
BlendStateGL4::Impl::Impl(BlendDescription const& description)
	// Blend factor
	: blendColor(description.BlendFactor)
	// Alpha data
	, alphaFunction(ToBlendFunctionGL4(description.AlphaBlendFunction))
	, alphaSource(ToBlendGL4(description.AlphaSourceBlend))
	, alphaDestination(ToBlendGL4(description.AlphaDestinationBlend))
	// Color data
	, colorFunction(ToBlendFunctionGL4(description.ColorBlendFunction))
	, colorSource(ToBlendGL4(description.ColorSourceBlend))
	, colorDestination(ToBlendGL4(description.ColorDestinationBlend))
{
}
//-----------------------------------------------------------------------
void BlendStateGL4::Impl::Apply()
{
	glEnable(GL_BLEND);

	glBlendEquationSeparate(colorFunction.value, alphaFunction.value);
	glBlendFuncSeparate(
		colorSource.value,
		colorDestination.value,
		alphaSource.value,
		alphaDestination.value
	);

	glBlendColor(
		blendColor.r,
		blendColor.g,
		blendColor.b,
		blendColor.a
	);
	
	///@todo Not implemented alpha to coverage.
	//{
	//	// Alpha to coverage
	//	glEnable(GL_MULTISAMPLE);
	//	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	//}
}
//-----------------------------------------------------------------------
BlendStateGL4::BlendStateGL4(BlendDescription const& description)
	: impl(new Impl(description))
{
}
//-----------------------------------------------------------------------
BlendStateGL4::~BlendStateGL4()
{
}
//-----------------------------------------------------------------------
void BlendStateGL4::Apply()
{
	POMDOG_ASSERT(impl);
	impl->Apply();
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
