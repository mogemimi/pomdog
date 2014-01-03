//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_GL4_INPUTLAYOUTGL4_F0AF2367_CC09_4F30_B3CD_E1B0A3BE5C22_HPP
#define POMDOG_SRC_GL4_INPUTLAYOUTGL4_F0AF2367_CC09_4F30_B3CD_E1B0A3BE5C22_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "OpenGLPrerequisites.hpp"
#include <vector>
#include <limits>
#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>
#include <Pomdog/Utility/detail/Tagged.hpp>
#include "../RenderSystem/NativeInputLayout.hpp"
#include "../Utility/Optional.hpp"
#include "TypesafeGL4.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

class VertexBufferGL4;

namespace Tags {

struct ScalarDataTypeTag {};

}// namespace Tags

using ScalarTypeGL4 = Tagged<GLuint, Tags::ScalarDataTypeTag>;
using VertexArrayGL4 = Tagged<GLuint, InputLayout>;

struct InputElementGL4
{
	// Attribute location.
	GLuint StartSlot;

	// Specifies the scalar data type.
	ScalarTypeGL4 ScalarType;

	// Must be 1, 2, 3, and 4.
	std::int8_t Components;
	
	// Input element size.
	std::uint8_t ByteWidth;
};

struct InputBindingGL4
{
	std::vector<InputElementGL4> InputElements;
	std::uint32_t InstanceFrequency;
};

class InputLayoutGL4: public NativeInputLayout
{
public:
	InputLayoutGL4() = delete;
	
	explicit InputLayoutGL4(ShaderProgramGL4 const& shaderProgram);
	
	InputLayoutGL4(ShaderProgramGL4 const& shaderProgram, std::vector<VertexBufferBinding> const& vertexBufferBinding);
	
	~InputLayoutGL4();

	void Apply(std::vector<std::shared_ptr<VertexBuffer>> const& vertexBuffers);
	
private:
	std::vector<InputBindingGL4> inputBindings;
	Optional<VertexArrayGL4> inputLayout;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_GL4_INPUTLAYOUTGL4_F0AF2367_CC09_4F30_B3CD_E1B0A3BE5C22_HPP)
