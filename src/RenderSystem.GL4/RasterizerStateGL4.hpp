// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Utility/detail/Tagged.hpp"

namespace Pomdog {
namespace Detail {
namespace GL4 {

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
    bool scissorTestEnable;
};

} // namespace GL4
} // namespace Detail
} // namespace Pomdog
