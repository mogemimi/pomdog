// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/rasterizer_state_gl4.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/gpu/rasterizer_descriptor.h"
#include "pomdog/utility/assert.h"

namespace pomdog::gpu::detail::gl4 {
namespace {

FillModeGL4 ToFillModeGL4(const FillMode& fillMode) noexcept
{
    switch (fillMode) {
    case FillMode::Solid:
        return FillModeGL4{GL_FILL};
    case FillMode::Wireframe:
        return FillModeGL4{GL_LINE};
    }
    POMDOG_UNREACHABLE("Unsupported fill mode");
}

} // namespace

std::unique_ptr<Error>
RasterizerStateGL4::Initialize(const RasterizerDescriptor& descriptor) noexcept
{
    fillMode = ToFillModeGL4(descriptor.fillMode);
    cullMode = descriptor.cullMode;
    depthBias = static_cast<decltype(depthBias)>(descriptor.depthBias);
    slopeScaledDepthBias = descriptor.slopeScaledDepthBias;
    multisampleAntiAliasEnable = descriptor.multisampleEnable;
    return nullptr;
}

void RasterizerStateGL4::Apply()
{
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    {
        GLint frontFace;
        glGetIntegerv(GL_FRONT_FACE, &frontFace);
        POMDOG_ASSERT(GL_CW == frontFace);
    }
#endif

    // CullMode:
    switch (cullMode) {
    case CullMode::None:
        glDisable(GL_CULL_FACE);
        break;
    case CullMode::ClockwiseFace:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        break;
    case CullMode::CounterClockwiseFace:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        break;
    }

    // FillMode:
    glPolygonMode(GL_FRONT_AND_BACK, fillMode.value);
    POMDOG_CHECK_ERROR_GL4("glPolygonMode");

    // NOTE: Modern graphics APIs (Direct3D 12, Metal and Vulkan) don't include
    // a parameter like ScissorEnable to enable/disable scissor test. So we
    // always set scissorTestEnable to true in OpenGL 4.
    constexpr bool scissorTestEnable = true;

    // Scissor Test:
    if constexpr (scissorTestEnable) {
        glEnable(GL_SCISSOR_TEST);
    }
    else {
        glDisable(GL_SCISSOR_TEST);
    }

    // Depth bias:
    if (depthBias != 0 && slopeScaledDepthBias != 0) {
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(slopeScaledDepthBias, depthBias);
    }
    else {
        glDisable(GL_POLYGON_OFFSET_FILL);
    }

    // Multisample Anti-Aliasing:
    if (multisampleAntiAliasEnable) {
        glEnable(GL_MULTISAMPLE);
        POMDOG_CHECK_ERROR_GL4("glEnable");
    }
    else {
        glDisable(GL_MULTISAMPLE);
    }
}

} // namespace pomdog::gpu::detail::gl4
