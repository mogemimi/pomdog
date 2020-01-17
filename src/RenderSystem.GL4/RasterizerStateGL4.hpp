// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include "../Utility/Tagged.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"

namespace Pomdog::Detail::GL4 {

using FillModeGL4 = Tagged<GLenum, FillMode>;

class RasterizerStateGL4 final {
public:
    RasterizerStateGL4() = delete;

    explicit RasterizerStateGL4(const RasterizerDescription& description);

    void Apply();

private:
    FillModeGL4 fillMode;
    CullMode cullMode;
    GLfloat depthBias;
    GLfloat slopeScaledDepthBias;
    bool multisampleAntiAliasEnable;
};

} // namespace Pomdog::Detail::GL4
