// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/blend_state_gl4.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/blend_desc.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {
namespace {

[[nodiscard]] GLenum
toBlendGL4NonTypesafe(BlendFactor blend) noexcept
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

[[nodiscard]] GLenum
toBlendOperationGL4NonTypesafe(BlendOperation operation) noexcept
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

[[nodiscard]] BlendGL4
toBlendGL4(BlendFactor blend) noexcept
{
    return BlendGL4{toBlendGL4NonTypesafe(blend)};
}

[[nodiscard]] BlendOperationGL4
toBlendOperationGL4(BlendOperation operation) noexcept
{
    return BlendOperationGL4{toBlendOperationGL4NonTypesafe(operation)};
}

void toRenderTargetBlendGL4(
    const RenderTargetBlendDesc& desc,
    RenderTargetBlendDescGL4& result) noexcept
{
    result.colorSource = toBlendGL4(desc.colorSourceBlend);
    result.colorDestination = toBlendGL4(desc.colorDestinationBlend);
    result.colorOperation = toBlendOperationGL4(desc.colorBlendOperation);
    result.alphaSource = toBlendGL4(desc.alphaSourceBlend);
    result.alphaDestination = toBlendGL4(desc.alphaDestinationBlend);
    result.alphaOperation = toBlendOperationGL4(desc.alphaBlendOperation);
    result.blendEnable = desc.blendEnable;
}

} // namespace

std::unique_ptr<Error>
BlendStateGL4::initialize(const BlendDesc& descriptor) noexcept
{
    independentBlendEnable_ = descriptor.independentBlendEnable;
    alphaToCoverageEnable_ = descriptor.alphaToCoverageEnable;

    for (std::size_t i = 0; i < descriptor.renderTargets.size(); ++i) {
        POMDOG_ASSERT(i < renderTargets_.size());
        toRenderTargetBlendGL4(descriptor.renderTargets[i], renderTargets_[i]);
    }
    return nullptr;
}

void BlendStateGL4::apply()
{
    if (independentBlendEnable_) {
        GLuint index = 0;
        for (auto& renderTarget : renderTargets_) {
            if (renderTarget.blendEnable) {
                glEnablei(GL_BLEND, index);
                POMDOG_CHECK_ERROR_GL4("glEnablei");
            }
            else {
                glDisablei(GL_BLEND, index);
                POMDOG_CHECK_ERROR_GL4("glDisablei");
            }
            glBlendFuncSeparatei(
                index,
                renderTarget.colorSource.value,
                renderTarget.colorDestination.value,
                renderTarget.alphaSource.value,
                renderTarget.alphaDestination.value);
            POMDOG_CHECK_ERROR_GL4("glBlendFuncSeparatei");
            glBlendEquationSeparatei(
                index,
                renderTarget.colorOperation.value,
                renderTarget.alphaOperation.value);
            POMDOG_CHECK_ERROR_GL4("glBlendEquationSeparatei");
            ++index;
        }
    }
    else {
        auto& renderTarget = renderTargets_.front();
        if (renderTarget.blendEnable) {
            glEnable(GL_BLEND);
        }
        else {
            glDisable(GL_BLEND);
        }
        glBlendEquationSeparate(
            renderTarget.colorOperation.value,
            renderTarget.alphaOperation.value);
        POMDOG_CHECK_ERROR_GL4("glBlendEquationSeparate");
        glBlendFuncSeparate(
            renderTarget.colorSource.value,
            renderTarget.colorDestination.value,
            renderTarget.alphaSource.value,
            renderTarget.alphaDestination.value);
        POMDOG_CHECK_ERROR_GL4("glBlendFuncSeparate");
    }

    if (alphaToCoverageEnable_) {
        glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    }
    else {
        glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    }
}

} // namespace pomdog::gpu::detail::gl4
