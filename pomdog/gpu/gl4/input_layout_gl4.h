// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/gpu/gl4/typesafe_gl4.h"
#include "pomdog/gpu/gl4/vertex_buffer_binding_gl4.h"
#include "pomdog/utility/tagged.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <memory>
#include <optional>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::gpu {
struct InputLayoutDesc;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::gl4 {

namespace Tags {

struct ScalarDataTypeTag final {
};
struct VertexArrayTag final {
};

} // namespace Tags

/// ScalarTypeGL4 is a type-safe wrapper around a GL scalar data type enum.
using ScalarTypeGL4 = pomdog::detail::Tagged<GLuint, Tags::ScalarDataTypeTag>;

/// VertexArrayGL4 is a type-safe wrapper around a GL vertex array object name.
using VertexArrayGL4 = pomdog::detail::Tagged<GLuint, Tags::VertexArrayTag>;

/// InputElementGL4 describes a single OpenGL vertex attribute binding.
struct InputElementGL4 final {
    GLuint attributeLocation;

    // Input element offset.
    u32 byteOffset;

    // Specifies the scalar data type.
    ScalarTypeGL4 scalarType;

    // Must be 1, 2, 3, and 4.
    i8 components;

    bool isInteger = false;

    bool normalized = false;
};

/// VertexBufferLayoutGL4 describes the layout of one vertex buffer binding slot.
struct VertexBufferLayoutGL4 final {
    std::vector<InputElementGL4> elements;

    u32 strideBytes = 0;

    u32 instanceStepRate = 0;

    u16 inputSlot = 0;
};

/// InputLayoutGL4 manages a GL vertex array object and maps vertex buffer bindings to shader attributes.
class InputLayoutGL4 final {
private:
    std::vector<VertexBufferLayoutGL4> vertexBuffers_;
    std::optional<VertexArrayGL4> inputLayout_;

public:
    InputLayoutGL4();

    ~InputLayoutGL4();

    /// Builds vertex attribute descriptions from the shader program and input layout descriptor.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const ShaderProgramGL4& shaderProgram,
        const InputLayoutDesc& descriptor) noexcept;

    /// Binds the vertex array object and streams vertex buffer data to the active VAO.
    void apply(const std::array<VertexBufferBindingGL4, 8>& vertexBuffers);
};

} // namespace pomdog::gpu::detail::gl4
