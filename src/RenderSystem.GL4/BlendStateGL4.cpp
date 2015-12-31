// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "BlendStateGL4.hpp"
#include "ErrorChecker.hpp"
#include "Pomdog/Graphics/BlendDescription.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {
namespace Detail {
namespace GL4 {
namespace {

static GLenum ToBlendGL4NonTypesafe(Blend blend) noexcept
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
    case Blend::Source1Color: return GL_SRC1_COLOR;
    case Blend::InverseSource1Color: return GL_ONE_MINUS_SRC1_COLOR;
    case Blend::Source1Alpha: return GL_SRC1_ALPHA;
    case Blend::InverseSource1Alpha: return GL_ONE_MINUS_SRC1_ALPHA;
    }
#ifdef _MSC_VER
    return GL_ZERO;
#endif
}
//-----------------------------------------------------------------------
static GLenum ToBlendFunctionGL4NonTypesafe(BlendFunction func) noexcept
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
    return BlendGL4{ToBlendGL4NonTypesafe(blend)};
}
//-----------------------------------------------------------------------
static BlendFunctionGL4 ToBlendFunctionGL4(BlendFunction func)
{
    return BlendFunctionGL4{ToBlendFunctionGL4NonTypesafe(func)};
}
//-----------------------------------------------------------------------
static void ToRenderTargetBlendGL4(RenderTargetBlendDescription const& desc,
    RenderTargetBlendDescGL4 & result)
{
    result.ColorSource = ToBlendGL4(desc.ColorSourceBlend);
    result.ColorDestination = ToBlendGL4(desc.ColorDestinationBlend);
    result.ColorFunction = ToBlendFunctionGL4(desc.ColorBlendFunction);
    result.AlphaSource = ToBlendGL4(desc.AlphaSourceBlend);
    result.AlphaDestination = ToBlendGL4(desc.AlphaDestinationBlend);
    result.AlphaFunction = ToBlendFunctionGL4(desc.AlphaBlendFunction);
    result.BlendEnable = desc.BlendEnable;
}

} // unnamed namespace
//-----------------------------------------------------------------------
BlendStateGL4::BlendStateGL4(BlendDescription const& description)
    : independentBlendEnable(description.IndependentBlendEnable)
{
    for (std::size_t i = 0; i < description.RenderTargets.size(); ++i) {
        POMDOG_ASSERT(i < renderTargets.size());
        ToRenderTargetBlendGL4(description.RenderTargets[i], renderTargets[i]);
    }
}
//-----------------------------------------------------------------------
void BlendStateGL4::Apply()
{
    if (independentBlendEnable) {
        GLuint index = 0;
        for (auto & renderTarget : renderTargets) {
            if (renderTarget.BlendEnable) {
                glEnablei(GL_BLEND, index);
                POMDOG_CHECK_ERROR_GL4("glEnablei");
            }
            else {
                glDisablei(GL_BLEND, index);
                POMDOG_CHECK_ERROR_GL4("glDisablei");
            }
            glBlendFuncSeparatei(
                index,
                renderTarget.ColorSource.value,
                renderTarget.ColorDestination.value,
                renderTarget.AlphaSource.value,
                renderTarget.AlphaDestination.value);
            POMDOG_CHECK_ERROR_GL4("glBlendFuncSeparatei");
            glBlendEquationSeparatei(
                index,
                renderTarget.ColorFunction.value,
                renderTarget.AlphaFunction.value);
            POMDOG_CHECK_ERROR_GL4("glBlendEquationSeparatei");
            ++index;
        }
    }
    else {
        auto & renderTarget = renderTargets.front();
        if (renderTarget.BlendEnable) {
            glEnable(GL_BLEND);
        }
        else {
            glDisable(GL_BLEND);
        }
        glBlendEquationSeparate(
            renderTarget.ColorFunction.value,
            renderTarget.AlphaFunction.value);
        POMDOG_CHECK_ERROR_GL4("glBlendEquationSeparate");
        glBlendFuncSeparate(
            renderTarget.ColorSource.value,
            renderTarget.ColorDestination.value,
            renderTarget.AlphaSource.value,
            renderTarget.AlphaDestination.value);
        POMDOG_CHECK_ERROR_GL4("glBlendFuncSeparate");
    }

    ///@todo Not implemented alpha to coverage.
    //{
    //    // Alpha to coverage
    //    glEnable(GL_MULTISAMPLE);
    //    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    //}
}
//-----------------------------------------------------------------------
} // namespace GL4
} // namespace Detail
} // namespace Pomdog
