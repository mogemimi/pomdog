// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/cull_mode.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/tagged.h"

namespace pomdog::gpu {
struct RasterizerDesc;
enum class FillMode : u8;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::gl4 {

using FillModeGL4 = pomdog::detail::Tagged<GLenum, FillMode>;

class RasterizerStateGL4 final {
private:
#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
    FillModeGL4 fillMode_ = GL_FILL;
#endif
    GLfloat depthBias_ = 0.0f;
    GLfloat slopeScaledDepthBias_ = 0.0f;
    CullMode cullMode_ = CullMode::CounterClockwiseFace;
    bool multisampleAntiAliasEnable_ = false;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(const RasterizerDesc& descriptor) noexcept;

    void apply();
};

} // namespace pomdog::gpu::detail::gl4
