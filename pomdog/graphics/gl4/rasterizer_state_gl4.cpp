// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "pomdog/graphics/gl4/rasterizer_state_gl4.hpp"
#include "pomdog/basic/unreachable.hpp"
#include "pomdog/graphics/gl4/error_checker.hpp"
#include "pomdog/graphics/rasterizer_description.hpp"
#include "pomdog/utility/assert.hpp"

namespace Pomdog::Detail::GL4 {
namespace {

FillModeGL4 ToFillModeGL4(const FillMode& fillMode) noexcept
{
    switch (fillMode) {
    case FillMode::Solid:
        return FillModeGL4{GL_FILL};
    case FillMode::WireFrame:
        return FillModeGL4{GL_LINE};
    }
    POMDOG_UNREACHABLE("Unsupported fill mode");
}

} // namespace

std::unique_ptr<Error>
RasterizerStateGL4::Initialize(const RasterizerDescription& description) noexcept
{
    fillMode = ToFillModeGL4(description.FillMode);
    cullMode = description.CullMode;
    depthBias = static_cast<decltype(depthBias)>(description.DepthBias);
    slopeScaledDepthBias = description.SlopeScaledDepthBias;
    multisampleAntiAliasEnable = description.MultisampleEnable;
    return nullptr;
}

void RasterizerStateGL4::Apply()
{
#if defined(DEBUG) && !defined(NDEBUG)
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

} // namespace Pomdog::Detail::GL4
