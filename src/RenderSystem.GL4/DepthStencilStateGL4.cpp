// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "DepthStencilStateGL4.hpp"
#include "ErrorChecker.hpp"
#include "../Basic/Unreachable.hpp"
#include "Pomdog/Graphics/DepthStencilDescription.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace GL4 {

namespace {

GLenum ToComparisonFunctionGL4NonTypesafe(ComparisonFunction comparison) noexcept
{
    switch (comparison) {
    case ComparisonFunction::Always: return GL_ALWAYS;
    case ComparisonFunction::Equal: return GL_EQUAL;
    case ComparisonFunction::Greater: return GL_GREATER;
    case ComparisonFunction::GreaterEqual: return GL_GEQUAL;
    case ComparisonFunction::Less: return GL_LESS;
    case ComparisonFunction::LessEqual: return GL_LEQUAL;
    case ComparisonFunction::Never: return GL_NEVER;
    case ComparisonFunction::NotEqual: return GL_NOTEQUAL;
    }
    POMDOG_UNREACHABLE("Unsupported comparison function");
}

GLenum ToStencilOperationGL4NonTypesafe(StencilOperation operation) noexcept
{
    // **NOTE**
    // `GL_DECR` clamps to 0.
    // `GL_INCR` clamps to the maximum representable unsigned value.

    switch (operation) {
    case StencilOperation::Decrement: return GL_DECR_WRAP;
    case StencilOperation::DecrementSaturation: return GL_DECR;
    case StencilOperation::Increment: return GL_INCR_WRAP;
    case StencilOperation::IncrementSaturation: return GL_INCR;
    case StencilOperation::Invert: return GL_INVERT;
    case StencilOperation::Keep: return GL_KEEP;
    case StencilOperation::Replace: return GL_REPLACE;
    case StencilOperation::Zero: return GL_ZERO;
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
    DepthStencilFaceOperationGL4 & result) noexcept
{
    result.stencilFunction = ToComparisonFunctionGL4(face.StencilFunction);
    result.stencilDepthBufferFail = ToStencilOperationGL4(face.StencilDepthBufferFail);
    result.stencilFail = ToStencilOperationGL4(face.StencilFail);
    result.stencilPass = ToStencilOperationGL4(face.StencilPass);
}

} // unnamed namespace

DepthStencilStateGL4::DepthStencilStateGL4(const DepthStencilDescription& description)
    : depthFunction(ToComparisonFunctionGL4(description.DepthBufferFunction))
    , referenceStencil(description.ReferenceStencil)
    , stencilMask(description.StencilMask)
    , stencilWriteMask(description.StencilWriteMask)
    , depthBufferWriteEnable(description.DepthBufferWriteEnable ? GL_TRUE: GL_FALSE)
    , stencilEnable(description.StencilEnable)
    , depthBufferEnable(description.DepthBufferEnable)
{
    static_assert(std::is_same<GLuint, std::uint32_t>::value, "");
    static_assert(std::is_same<GLint, std::int32_t>::value, "");

    ToDepthStencilFaceOperationGL4(description.ClockwiseFace, clockwiseFace);
    ToDepthStencilFaceOperationGL4(description.CounterClockwiseFace, counterClockwiseFace);
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

#if defined(DEBUG) && !defined(NDEBUG)
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

} // namespace GL4
} // namespace Detail
} // namespace Pomdog
