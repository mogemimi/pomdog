// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "BlendStateGL4.hpp"
#include "ErrorChecker.hpp"
#include "../Basic/Unreachable.hpp"
#include "Pomdog/Graphics/BlendDescription.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {
namespace Detail {
namespace GL4 {
namespace {

GLenum ToBlendGL4NonTypesafe(Blend blend) noexcept
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
    // case Blend::BlendFactorAlpha: return GL_CONSTANT_ALPHA;
    // case Blend::InvereseBlendFactorAlpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
    case Blend::Source1Color: return GL_SRC1_COLOR;
    case Blend::InverseSource1Color: return GL_ONE_MINUS_SRC1_COLOR;
    case Blend::Source1Alpha: return GL_SRC1_ALPHA;
    case Blend::InverseSource1Alpha: return GL_ONE_MINUS_SRC1_ALPHA;
    }
    POMDOG_UNREACHABLE("Unsupported blend factor");
}

GLenum ToBlendOperationGL4NonTypesafe(BlendOperation operation) noexcept
{
    switch (operation) {
    case BlendOperation::Add: return GL_FUNC_ADD;
    case BlendOperation::Subtract: return GL_FUNC_SUBTRACT;
    case BlendOperation::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
    case BlendOperation::Min: return GL_MIN;
    case BlendOperation::Max: return GL_MAX;
    }
    POMDOG_UNREACHABLE("Unsupported blend operation");
}

BlendGL4 ToBlendGL4(Blend blend) noexcept
{
    return BlendGL4{ToBlendGL4NonTypesafe(blend)};
}

BlendOperationGL4 ToBlendOperationGL4(BlendOperation operation) noexcept
{
    return BlendOperationGL4{ToBlendOperationGL4NonTypesafe(operation)};
}

void ToRenderTargetBlendGL4(
    const RenderTargetBlendDescription& desc,
    RenderTargetBlendDescGL4 & result) noexcept
{
    result.ColorSource = ToBlendGL4(desc.ColorSourceBlend);
    result.ColorDestination = ToBlendGL4(desc.ColorDestinationBlend);
    result.ColorOperation = ToBlendOperationGL4(desc.ColorBlendOperation);
    result.AlphaSource = ToBlendGL4(desc.AlphaSourceBlend);
    result.AlphaDestination = ToBlendGL4(desc.AlphaDestinationBlend);
    result.AlphaOperation = ToBlendOperationGL4(desc.AlphaBlendOperation);
    result.BlendEnable = desc.BlendEnable;
}

} // unnamed namespace

BlendStateGL4::BlendStateGL4(const BlendDescription& description)
    : independentBlendEnable(description.IndependentBlendEnable)
    , alphaToCoverageEnable(description.AlphaToCoverageEnable)
{
    for (std::size_t i = 0; i < description.RenderTargets.size(); ++i) {
        POMDOG_ASSERT(i < renderTargets.size());
        ToRenderTargetBlendGL4(description.RenderTargets[i], renderTargets[i]);
    }
}

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
                renderTarget.ColorOperation.value,
                renderTarget.AlphaOperation.value);
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
            renderTarget.ColorOperation.value,
            renderTarget.AlphaOperation.value);
        POMDOG_CHECK_ERROR_GL4("glBlendEquationSeparate");
        glBlendFuncSeparate(
            renderTarget.ColorSource.value,
            renderTarget.ColorDestination.value,
            renderTarget.AlphaSource.value,
            renderTarget.AlphaDestination.value);
        POMDOG_CHECK_ERROR_GL4("glBlendFuncSeparate");
    }

    if (alphaToCoverageEnable) {
        glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    }
    else {
        glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    }
}

} // namespace GL4
} // namespace Detail
} // namespace Pomdog
