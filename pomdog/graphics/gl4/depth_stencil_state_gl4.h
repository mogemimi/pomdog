// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/graphics/gl4/opengl_prerequisites.hpp"
#include "pomdog/utility/errors.hpp"
#include "pomdog/utility/tagged.hpp"

namespace pomdog::detail::gl4 {

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
    [[nodiscard]] std::unique_ptr<Error>
    Initialize(const DepthStencilDescription& description) noexcept;

    void Apply();

private:
    void ApplyDepthTest();
    void ApplyStencilTest();

private:
    DepthStencilFaceOperationGL4 clockwiseFace;
    DepthStencilFaceOperationGL4 counterClockwiseFace;
    ComparisonFunctionGL4 depthFunction;

    GLint referenceStencil = 0;
    GLuint stencilMask = 0;
    GLuint stencilWriteMask = 0;
    GLboolean depthBufferWriteEnable = GL_FALSE;
    bool stencilEnable = false;
    bool depthBufferEnable = false;
};

} // namespace pomdog::detail::gl4
