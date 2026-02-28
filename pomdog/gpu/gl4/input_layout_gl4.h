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

namespace pomdog::gpu {
struct InputLayoutDescriptor;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::gl4 {

namespace Tags {

struct ScalarDataTypeTag final {
};
struct VertexArrayTag final {
};

} // namespace Tags

using ScalarTypeGL4 = pomdog::detail::Tagged<GLuint, Tags::ScalarDataTypeTag>;
using VertexArrayGL4 = pomdog::detail::Tagged<GLuint, Tags::VertexArrayTag>;

struct InputElementGL4 final {
    GLuint attributeLocation;

    // Input element offset.
    u32 byteOffset;

    u16 inputSlot;

    u16 instanceStepRate;

    // Specifies the scalar data type.
    ScalarTypeGL4 scalarType;

    // Must be 1, 2, 3, and 4.
    i8 components;

    bool isInteger = false;
};

class InputLayoutGL4 final {
private:
    std::vector<InputElementGL4> inputElements_;
    std::optional<VertexArrayGL4> inputLayout_;

public:
    explicit InputLayoutGL4(const ShaderProgramGL4& shaderProgram);

    InputLayoutGL4(
        const ShaderProgramGL4& shaderProgram,
        const InputLayoutDescriptor& descriptor);

    ~InputLayoutGL4();

    void apply(const std::array<VertexBufferBindingGL4, 8>& vertexBuffers);
};

} // namespace pomdog::gpu::detail::gl4
