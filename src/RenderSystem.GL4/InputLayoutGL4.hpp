// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_INPUTLAYOUTGL4_F0AF2367_HPP
#define POMDOG_INPUTLAYOUTGL4_F0AF2367_HPP

#include "OpenGLPrerequisites.hpp"
#include "TypesafeGL4.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Utility/detail/Tagged.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <vector>
#include <memory>

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
    explicit InputLayoutGL4(ShaderProgramGL4 const& shaderProgram);

    InputLayoutGL4(ShaderProgramGL4 const& shaderProgram,
        InputLayoutDescription const& description);

    ~InputLayoutGL4();

    void Apply(std::vector<VertexBufferBinding> const& vertexBuffers);

private:
    std::vector<InputElementGL4> inputElements;
    std::vector<VertexDeclarationGL4> vertexDeclarations;
    Optional<VertexArrayGL4> inputLayout;
};

} // namespace GL4
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_INPUTLAYOUTGL4_F0AF2367_HPP
