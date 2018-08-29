// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include "TypesafeGL4.hpp"
#include "../Utility/Tagged.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>
#include <optional>
#include <vector>

namespace Pomdog {
namespace Detail {
namespace GL4 {

namespace Tags {

struct ScalarDataTypeTag {};
struct VertexArrayTag {};

} // namespace Tags

using ScalarTypeGL4 = Tagged<GLuint, Tags::ScalarDataTypeTag>;
using VertexArrayGL4 = Tagged<GLuint, Tags::VertexArrayTag>;

struct InputElementGL4 {
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

struct VertexDeclarationGL4 {
    GLsizei StrideBytes;
};

class InputLayoutGL4 final {
public:
    explicit InputLayoutGL4(const ShaderProgramGL4& shaderProgram);

    InputLayoutGL4(
        const ShaderProgramGL4& shaderProgram,
        const InputLayoutDescription& description);

    ~InputLayoutGL4();

    void Apply(const std::vector<VertexBufferBinding>& vertexBuffers);

private:
    std::vector<InputElementGL4> inputElements;
    std::vector<VertexDeclarationGL4> vertexDeclarations;
    std::optional<VertexArrayGL4> inputLayout;
};

} // namespace GL4
} // namespace Detail
} // namespace Pomdog
