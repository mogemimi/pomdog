// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/depth_stencil_state_gl4.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/depth_stencil_descriptor.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/gpu/gl4/format_helper.h"
#include "pomdog/utility/assert.h"

namespace pomdog::gpu::detail::gl4 {
namespace {

GLenum ToStencilOperationGL4NonTypesafe(StencilOperation operation) noexcept
{
    // **NOTE**
    // `GL_DECR` clamps to 0.
    // `GL_INCR` clamps to the maximum representable unsigned value.

    switch (operation) {
    case StencilOperation::Decrement:
        return GL_DECR_WRAP;
    case StencilOperation::DecrementSaturation:
        return GL_DECR;
    case StencilOperation::Increment:
        return GL_INCR_WRAP;
    case StencilOperation::IncrementSaturation:
        return GL_INCR;
    case StencilOperation::Invert:
        return GL_INVERT;
    case StencilOperation::Keep:
        return GL_KEEP;
    case StencilOperation::Replace:
        return GL_REPLACE;
    case StencilOperation::Zero:
        return GL_ZERO;
    }
    POMDOG_UNREACHABLE("Unsupported stencil operation");
}

ComparisonFunctionGL4
ToComparisonFunctionGL4(const ComparisonFunction& comparison) noexcept
{
    return ComparisonFunctionGL4{
        ToComparisonFunctionGL4NonTypesafe(comparison)};
}

StencilOperationGL4
ToStencilOperationGL4(const StencilOperation& operation) noexcept
{
    return StencilOperationGL4{
        ToStencilOperationGL4NonTypesafe(operation)};
}

void ToDepthStencilFaceOperationGL4(
    const DepthStencilOperation& face,
    DepthStencilFaceOperationGL4& result) noexcept
{
    result.stencilFunction = ToComparisonFunctionGL4(face.stencilFunction);
    result.stencilDepthBufferFail = ToStencilOperationGL4(face.stencilDepthBufferFail);
    result.stencilFail = ToStencilOperationGL4(face.stencilFail);
    result.stencilPass = ToStencilOperationGL4(face.stencilPass);
}

} // namespace

std::unique_ptr<Error>
DepthStencilStateGL4::Initialize(const DepthStencilDescriptor& descriptor) noexcept
{
    static_assert(std::is_same<GLuint, std::uint32_t>::value, "");
    static_assert(std::is_same<GLint, std::int32_t>::value, "");

    depthFunction = ToComparisonFunctionGL4(descriptor.depthBufferFunction);
    referenceStencil = descriptor.referenceStencil;
    stencilMask = descriptor.stencilMask;
    stencilWriteMask = descriptor.stencilWriteMask;
    depthBufferWriteEnable = descriptor.depthBufferWriteEnable ? GL_TRUE : GL_FALSE;
    stencilEnable = descriptor.stencilEnable;
    depthBufferEnable = descriptor.depthBufferEnable;

    ToDepthStencilFaceOperationGL4(descriptor.clockwiseFace, clockwiseFace);
    ToDepthStencilFaceOperationGL4(descriptor.counterClockwiseFace, counterClockwiseFace);

    return nullptr;
}

void DepthStencilStateGL4::ApplyDepthTest()
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
    POMDOG_ASSERT(depthBufferWriteEnable == GL_TRUE || depthBufferWriteEnable == GL_FALSE);
    glDepthMask(depthBufferWriteEnable);

    // depth function
    glDepthFunc(depthFunction.value);
    POMDOG_CHECK_ERROR_GL4("glDepthFunc");
}

void DepthStencilStateGL4::ApplyStencilTest()
{
    if (!stencilEnable) {
        glDisable(GL_STENCIL_TEST);
        return;
    }

    glEnable(GL_STENCIL_TEST);

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    {
        GLint frontFace;
        glGetIntegerv(GL_FRONT_FACE, &frontFace);
        POMDOG_ASSERT(GL_CW == frontFace);
    }
#endif

    // ClockwiseFace:
    glStencilFuncSeparate(GL_FRONT,
        clockwiseFace.stencilFunction.value,
        referenceStencil,
        stencilMask);

    glStencilOpSeparate(GL_FRONT,
        clockwiseFace.stencilFail.value,
        clockwiseFace.stencilDepthBufferFail.value,
        clockwiseFace.stencilPass.value);

    // CounterClockwiseFace:
    glStencilFuncSeparate(GL_BACK,
        counterClockwiseFace.stencilFunction.value,
        referenceStencil,
        stencilMask);

    glStencilOpSeparate(GL_BACK,
        counterClockwiseFace.stencilFail.value,
        counterClockwiseFace.stencilDepthBufferFail.value,
        counterClockwiseFace.stencilPass.value);

    glStencilMask(stencilWriteMask);
    POMDOG_CHECK_ERROR_GL4("glStencilMask");
}

void DepthStencilStateGL4::Apply()
{
    ApplyDepthTest();
    ApplyStencilTest();
}

} // namespace pomdog::gpu::detail::gl4
