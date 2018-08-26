// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include "../Utility/Tagged.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"

namespace Pomdog {
namespace Detail {
namespace GL4 {

using ComparisonFunctionGL4 = Tagged<GLenum, ComparisonFunction>;
using StencilOperationGL4 = Tagged<GLenum, StencilOperation>;

struct DepthStencilFaceOperationGL4 final {
    ComparisonFunctionGL4 stencilFunction = GL_ALWAYS;
    StencilOperationGL4 stencilFail = GL_KEEP;
    StencilOperationGL4 stencilDepthBufferFail = GL_KEEP;
    StencilOperationGL4 stencilPass = GL_KEEP;
};

class DepthStencilStateGL4 final {
public:
    DepthStencilStateGL4() = delete;

    explicit DepthStencilStateGL4(const DepthStencilDescription& description);

    void Apply();

private:
    void ApplyDepthTest();
    void ApplyStencilTest();

private:
    DepthStencilFaceOperationGL4 clockwiseFace;
    DepthStencilFaceOperationGL4 counterClockwiseFace;
    ComparisonFunctionGL4 depthFunction;

    GLint referenceStencil;
    GLuint stencilMask;
    GLuint stencilWriteMask;
    GLboolean depthBufferWriteEnable;
    bool stencilEnable;
    bool depthBufferEnable;
};

} // namespace GL4
} // namespace Detail
} // namespace Pomdog
