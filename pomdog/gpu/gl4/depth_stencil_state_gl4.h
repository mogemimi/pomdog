// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/tagged.h"

namespace pomdog::gpu {
struct DepthStencilDesc;
enum class ComparisonFunction : u8;
enum class StencilOperation : u8;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::gl4 {

using ComparisonFunctionGL4 = pomdog::detail::Tagged<GLenum, ComparisonFunction>;
using StencilOperationGL4 = pomdog::detail::Tagged<GLenum, StencilOperation>;

struct DepthStencilFaceOperationGL4 final {
    ComparisonFunctionGL4 stencilFunction = GL_ALWAYS;
    StencilOperationGL4 stencilFail = GL_KEEP;
    StencilOperationGL4 stencilDepthBufferFail = GL_KEEP;
    StencilOperationGL4 stencilPass = GL_KEEP;
};

class DepthStencilStateGL4 final {
private:
    DepthStencilFaceOperationGL4 clockwiseFace_;
    DepthStencilFaceOperationGL4 counterClockwiseFace_;
    ComparisonFunctionGL4 depthFunction_;

    GLint referenceStencil_ = 0;
    GLuint stencilMask_ = 0;
    GLuint stencilWriteMask_ = 0;
    GLboolean depthBufferWriteEnable_ = GL_FALSE;
    bool stencilEnable_ = false;
    bool depthBufferEnable_ = false;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(const DepthStencilDesc& descriptor) noexcept;

    void apply();

private:
    void applyDepthTest();
    void applyStencilTest();
};

} // namespace pomdog::gpu::detail::gl4
