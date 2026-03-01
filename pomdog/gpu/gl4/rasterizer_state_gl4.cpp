// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/rasterizer_state_gl4.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/gpu/rasterizer_descriptor.h"
#include "pomdog/utility/assert.h"

namespace pomdog::gpu::detail::gl4 {
namespace {

[[nodiscard]] FillModeGL4
toFillModeGL4(FillMode fillMode) noexcept
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
RasterizerStateGL4::initialize(const RasterizerDescriptor& descriptor) noexcept
{
    fillMode_ = toFillModeGL4(descriptor.fillMode);
    cullMode_ = descriptor.cullMode;
    depthBias_ = static_cast<decltype(depthBias_)>(descriptor.depthBias);
    slopeScaledDepthBias_ = descriptor.slopeScaledDepthBias;
    multisampleAntiAliasEnable_ = descriptor.multisampleEnable;
    return nullptr;
}

void RasterizerStateGL4::apply()
{
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    {
        GLint frontFace;
        glGetIntegerv(GL_FRONT_FACE, &frontFace);
        POMDOG_ASSERT(GL_CW == frontFace);
    }
#endif

    // CullMode:
    switch (cullMode_) {
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
    glPolygonMode(GL_FRONT_AND_BACK, fillMode_.value);
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
    if (depthBias_ != 0 || slopeScaledDepthBias_ != 0) {
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(slopeScaledDepthBias_, depthBias_);
    }
    else {
        glDisable(GL_POLYGON_OFFSET_FILL);
    }

    // Multisample Anti-Aliasing:
    if (multisampleAntiAliasEnable_) {
        glEnable(GL_MULTISAMPLE);
        POMDOG_CHECK_ERROR_GL4("glEnable");
    }
    else {
        glDisable(GL_MULTISAMPLE);
    }
}

} // namespace pomdog::gpu::detail::gl4
