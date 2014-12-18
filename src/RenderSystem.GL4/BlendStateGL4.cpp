//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "BlendStateGL4.hpp"
#include "ErrorChecker.hpp"
#include <Pomdog/Graphics/BlendDescription.hpp>
#include <Pomdog/Math/Vector4.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <utility>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {
//-----------------------------------------------------------------------
namespace {

static GLenum ToBlendGL4NonTypesafe(Blend blend)
{
	switch (blend) {
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
	switch (func) {
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
	return BlendGL4{
		ToBlendGL4NonTypesafe(blend)
	};
}
//-----------------------------------------------------------------------
static BlendFunctionGL4 ToBlendFunctionGL4(BlendFunction func)
{
	return BlendFunctionGL4{
		ToBlendFunctionGL4NonTypesafe(func)
	};
}

}// unnamed namespace
//-----------------------------------------------------------------------
BlendStateGL4::BlendStateGL4(BlendDescription const& description)
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
void BlendStateGL4::Apply(NativeGraphicsContext &)
{
	glEnable(GL_BLEND);

	glBlendEquationSeparate(colorFunction.value, alphaFunction.value);
	glBlendFuncSeparate(
		colorSource.value,
		colorDestination.value,
		alphaSource.value,
		alphaDestination.value
	);
	
	#ifdef DEBUG
	ErrorChecker::CheckError("glBlendFuncSeparate", __FILE__, __LINE__);
	#endif

	auto colorVector = blendColor.ToVector4();
	glBlendColor(colorVector.X, colorVector.Y, colorVector.Z, colorVector.W);
	
	#ifdef DEBUG
	ErrorChecker::CheckError("glBlendColor", __FILE__, __LINE__);
	#endif
	
	///@todo Not implemented alpha to coverage.
	//{
	//	// Alpha to coverage
	//	glEnable(GL_MULTISAMPLE);
	//	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	//}
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
