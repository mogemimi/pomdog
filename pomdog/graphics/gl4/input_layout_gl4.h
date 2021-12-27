// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/graphics/gl4/opengl_prerequisites.hpp"
#include "pomdog/graphics/gl4/typesafe_gl4.hpp"
#include "pomdog/graphics/gl4/vertex_buffer_binding_gl4.hpp"
#include "pomdog/utility/tagged.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <memory>
#include <optional>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::gl4 {

namespace Tags {

struct ScalarDataTypeTag {
};
struct VertexArrayTag {
};

} // namespace Tags

using ScalarTypeGL4 = Tagged<GLuint, Tags::ScalarDataTypeTag>;
using VertexArrayGL4 = Tagged<GLuint, Tags::VertexArrayTag>;

struct InputElementGL4 final {
    GLuint AttributeLocation;

    // Input element offset.
    std::uint32_t ByteOffset;

    std::uint16_t InputSlot;

    std::uint16_t InstanceStepRate;

    // Specifies the scalar data type.
    ScalarTypeGL4 ScalarType;

    // Must be 1, 2, 3, and 4.
    std::int8_t Components;

    bool IsInteger = false;
};

class InputLayoutGL4 final {
public:
    explicit InputLayoutGL4(const ShaderProgramGL4& shaderProgram);

    InputLayoutGL4(
        const ShaderProgramGL4& shaderProgram,
        const InputLayoutDescription& description);

    ~InputLayoutGL4();

    void Apply(const std::array<VertexBufferBindingGL4, 8>& vertexBuffers);

private:
    std::vector<InputElementGL4> inputElements;
    std::optional<VertexArrayGL4> inputLayout;
};

} // namespace pomdog::detail::gl4
