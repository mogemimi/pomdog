// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include "../Utility/Tagged.hpp"
#include "Pomdog/Graphics/CullMode.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Utility/Errors.hpp"

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
