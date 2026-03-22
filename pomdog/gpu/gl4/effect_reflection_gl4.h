// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/gpu/gl4/typesafe_gl4.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {

struct UniformBlockGL4 final {
    std::string name;
    u32 byteSize;
    u32 blockIndex;
};

struct UniformGL4 final {
    std::string name;
    GLint location;
    GLenum type;
    GLuint arrayCount;
};

[[nodiscard]] std::vector<UniformBlockGL4>
enumerateUniformBlocks(const ShaderProgramGL4& shaderProgram) noexcept;

[[nodiscard]] std::vector<UniformGL4>
enumerateUniforms(const ShaderProgramGL4& shaderProgram) noexcept;

} // namespace pomdog::gpu::detail::gl4
