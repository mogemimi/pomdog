// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/cull_mode.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/tagged.h"

namespace pomdog::gpu::detail::gl4 {

using FillModeGL4 = pomdog::detail::Tagged<GLenum, FillMode>;

class RasterizerStateGL4 final {
public:
    [[nodiscard]] std::unique_ptr<Error>
    Initialize(const RasterizerDescriptor& descriptor) noexcept;

    void Apply();

private:
    FillModeGL4 fillMode = GL_FILL;
    GLfloat depthBias = 0.0f;
    GLfloat slopeScaledDepthBias = 0.0f;
    CullMode cullMode = CullMode::CounterClockwiseFace;
    bool multisampleAntiAliasEnable = false;
};

} // namespace pomdog::gpu::detail::gl4
