// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/blend_state_gl4.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/blend_descriptor.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {
namespace {

GLenum ToBlendGL4NonTypesafe(BlendFactor blend) noexcept
{
    switch (blend) {
    case BlendFactor::Zero:
        return GL_ZERO;
    case BlendFactor::One:
        return GL_ONE;
    case BlendFactor::SourceColor:
        return GL_SRC_COLOR;
    case BlendFactor::InverseSourceColor:
        return GL_ONE_MINUS_SRC_COLOR;
    case BlendFactor::SourceAlpha:
        return GL_SRC_ALPHA;
    case BlendFactor::InverseSourceAlpha:
        return GL_ONE_MINUS_SRC_ALPHA;
    case BlendFactor::DestinationAlpha:
        return GL_DST_ALPHA;
    case BlendFactor::InverseDestinationAlpha:
        return GL_ONE_MINUS_DST_ALPHA;
    case BlendFactor::DestinationColor:
        return GL_DST_COLOR;
    case BlendFactor::InverseDestinationColor:
        return GL_ONE_MINUS_DST_COLOR;
    case BlendFactor::SourceAlphaSaturation:
        return GL_SRC_ALPHA_SATURATE;
    case BlendFactor::BlendFactor:
        return GL_CONSTANT_COLOR;
    case BlendFactor::InvereseBlendFactor:
        return GL_ONE_MINUS_CONSTANT_COLOR;
    // case BlendFactor::BlendFactorAlpha: return GL_CONSTANT_ALPHA;
    // case BlendFactor::InvereseBlendFactorAlpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
    case BlendFactor::Source1Color:
        return GL_SRC1_COLOR;
    case BlendFactor::InverseSource1Color:
        return GL_ONE_MINUS_SRC1_COLOR;
    case BlendFactor::Source1Alpha:
        return GL_SRC1_ALPHA;
    case BlendFactor::InverseSource1Alpha:
        return GL_ONE_MINUS_SRC1_ALPHA;
    }
    POMDOG_UNREACHABLE("Unsupported blend factor");
}

GLenum ToBlendOperationGL4NonTypesafe(BlendOperation operation) noexcept
{
    switch (operation) {
    case BlendOperation::Add:
        return GL_FUNC_ADD;
    case BlendOperation::Subtract:
        return GL_FUNC_SUBTRACT;
    case BlendOperation::ReverseSubtract:
        return GL_FUNC_REVERSE_SUBTRACT;
    case BlendOperation::Min:
        return GL_MIN;
    case BlendOperation::Max:
        return GL_MAX;
    }
    POMDOG_UNREACHABLE("Unsupported blend operation");
}

BlendGL4 ToBlendGL4(BlendFactor blend) noexcept
{
    return BlendGL4{ToBlendGL4NonTypesafe(blend)};
}

BlendOperationGL4 ToBlendOperationGL4(BlendOperation operation) noexcept
{
    return BlendOperationGL4{ToBlendOperationGL4NonTypesafe(operation)};
}

void ToRenderTargetBlendGL4(
    const RenderTargetBlendDescriptor& desc,
    RenderTargetBlendDescriptorGL4& result) noexcept
{
    result.ColorSource = ToBlendGL4(desc.colorSourceBlend);
    result.ColorDestination = ToBlendGL4(desc.colorDestinationBlend);
    result.ColorOperation = ToBlendOperationGL4(desc.colorBlendOperation);
    result.AlphaSource = ToBlendGL4(desc.alphaSourceBlend);
    result.AlphaDestination = ToBlendGL4(desc.alphaDestinationBlend);
    result.AlphaOperation = ToBlendOperationGL4(desc.alphaBlendOperation);
    result.BlendEnable = desc.blendEnable;
}

} // namespace

std::unique_ptr<Error>
BlendStateGL4::Initialize(const BlendDescriptor& descriptor) noexcept
{
    independentBlendEnable = descriptor.independentBlendEnable;
    alphaToCoverageEnable = descriptor.alphaToCoverageEnable;

    for (std::size_t i = 0; i < descriptor.renderTargets.size(); ++i) {
        POMDOG_ASSERT(i < renderTargets.size());
        ToRenderTargetBlendGL4(descriptor.renderTargets[i], renderTargets[i]);
    }
    return nullptr;
}

void BlendStateGL4::Apply()
{
    if (independentBlendEnable) {
        GLuint index = 0;
        for (auto& renderTarget : renderTargets) {
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
        auto& renderTarget = renderTargets.front();
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

} // namespace pomdog::gpu::detail::gl4
