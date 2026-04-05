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

using ScalarTypeGL4 = pomdog::detail::Tagged<GLuint, Tags::ScalarDataTypeTag>;
using VertexArrayGL4 = pomdog::detail::Tagged<GLuint, Tags::VertexArrayTag>;

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

struct VertexBufferLayoutGL4 final {
    std::vector<InputElementGL4> elements;

    u32 strideBytes = 0;

    u32 instanceStepRate = 0;

    u16 inputSlot = 0;
};

class InputLayoutGL4 final {
private:
    std::vector<VertexBufferLayoutGL4> vertexBuffers_;
    std::optional<VertexArrayGL4> inputLayout_;

public:
    InputLayoutGL4();

    ~InputLayoutGL4();

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const ShaderProgramGL4& shaderProgram,
        const InputLayoutDesc& descriptor) noexcept;

    void apply(const std::array<VertexBufferBindingGL4, 8>& vertexBuffers);
};

} // namespace pomdog::gpu::detail::gl4
