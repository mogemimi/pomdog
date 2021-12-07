// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/cull_mode.hpp"
#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/graphics/gl4/opengl_prerequisites.hpp"
#include "pomdog/utility/errors.hpp"
#include "pomdog/utility/tagged.hpp"

namespace Pomdog::Detail::GL4 {

using FillModeGL4 = Tagged<GLenum, FillMode>;

class RasterizerStateGL4 final {
public:
    [[nodiscard]] std::unique_ptr<Error>
    Initialize(const RasterizerDescription& description) noexcept;

    void Apply();

private:
    FillModeGL4 fillMode = GL_FILL;
    GLfloat depthBias = 0.0f;
    GLfloat slopeScaledDepthBias = 0.0f;
    CullMode cullMode = CullMode::CounterClockwiseFace;
    bool multisampleAntiAliasEnable = false;
};

} // namespace Pomdog::Detail::GL4
