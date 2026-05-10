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

/// UniformBlockGL4 describes a uniform block in a linked GL shader program.
struct UniformBlockGL4 final {
    std::string name;
    u32 byteSize;
    u32 blockIndex;
};

/// UniformGL4 describes a single uniform variable in a linked GL shader program.
struct UniformGL4 final {
    std::string name;
    GLint location;
    GLenum type;
    GLuint arrayCount;
};

/// Returns all uniform blocks active in the shader program.
[[nodiscard]] std::vector<UniformBlockGL4>
enumerateUniformBlocks(const ShaderProgramGL4& shaderProgram) noexcept;

/// Returns all active uniforms in the shader program.
[[nodiscard]] std::vector<UniformGL4>
enumerateUniforms(const ShaderProgramGL4& shaderProgram) noexcept;

} // namespace pomdog::gpu::detail::gl4
