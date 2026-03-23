// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/depth_stencil_state_gl4.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/depth_stencil_desc.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/gpu/gl4/format_helper.h"
#include "pomdog/utility/assert.h"

namespace pomdog::gpu::detail::gl4 {
namespace {

[[nodiscard]] GLenum
toStencilOperationGL4NonTypesafe(StencilOperation operation) noexcept
{
    // NOTE:
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

[[nodiscard]] ComparisonFunctionGL4
toComparisonFunctionGL4(const ComparisonFunction& comparison) noexcept
{
    return ComparisonFunctionGL4{
        toComparisonFunctionGL4NonTypesafe(comparison)};
}

[[nodiscard]] StencilOperationGL4
toStencilOperationGL4(const StencilOperation& operation) noexcept
{
    return StencilOperationGL4{
        toStencilOperationGL4NonTypesafe(operation)};
}

void toDepthStencilFaceOperationGL4(
    const DepthStencilOperation& face,
    DepthStencilFaceOperationGL4& result) noexcept
{
    result.stencilFunction = toComparisonFunctionGL4(face.stencilFunction);
    result.stencilDepthBufferFail = toStencilOperationGL4(face.stencilDepthBufferFail);
    result.stencilFail = toStencilOperationGL4(face.stencilFail);
    result.stencilPass = toStencilOperationGL4(face.stencilPass);
}

} // namespace

std::unique_ptr<Error>
DepthStencilStateGL4::initialize(const DepthStencilDesc& descriptor) noexcept
{
    static_assert(std::is_same<GLuint, std::uint32_t>::value, "");
    static_assert(std::is_same<GLint, std::int32_t>::value, "");

    depthFunction_ = toComparisonFunctionGL4(descriptor.depthBufferFunction);
    referenceStencil_ = descriptor.referenceStencil;
    stencilMask_ = descriptor.stencilMask;
    stencilWriteMask_ = descriptor.stencilWriteMask;
    depthBufferWriteEnable_ = descriptor.depthBufferWriteEnable ? GL_TRUE : GL_FALSE;
    stencilEnable_ = descriptor.stencilEnable;
    depthBufferEnable_ = descriptor.depthBufferEnable;

    toDepthStencilFaceOperationGL4(descriptor.clockwiseFace, clockwiseFace_);
    toDepthStencilFaceOperationGL4(descriptor.counterClockwiseFace, counterClockwiseFace_);

    return nullptr;
}

void DepthStencilStateGL4::applyDepthTest()
{
    if (!depthBufferEnable_) {
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
    POMDOG_ASSERT(depthBufferWriteEnable_ == GL_TRUE || depthBufferWriteEnable_ == GL_FALSE);
    glDepthMask(depthBufferWriteEnable_);

    // depth function
    glDepthFunc(depthFunction_.value);
    POMDOG_CHECK_ERROR_GL4("glDepthFunc");
}

void DepthStencilStateGL4::applyStencilTest()
{
    if (!stencilEnable_) {
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
        clockwiseFace_.stencilFunction.value,
        referenceStencil_,
        stencilMask_);

    glStencilOpSeparate(GL_FRONT,
        clockwiseFace_.stencilFail.value,
        clockwiseFace_.stencilDepthBufferFail.value,
        clockwiseFace_.stencilPass.value);

    // CounterClockwiseFace:
    glStencilFuncSeparate(GL_BACK,
        counterClockwiseFace_.stencilFunction.value,
        referenceStencil_,
        stencilMask_);

    glStencilOpSeparate(GL_BACK,
        counterClockwiseFace_.stencilFail.value,
        counterClockwiseFace_.stencilDepthBufferFail.value,
        counterClockwiseFace_.stencilPass.value);

    glStencilMask(stencilWriteMask_);
    POMDOG_CHECK_ERROR_GL4("glStencilMask");
}

void DepthStencilStateGL4::apply()
{
    applyDepthTest();
    applyStencilTest();
}

} // namespace pomdog::gpu::detail::gl4
