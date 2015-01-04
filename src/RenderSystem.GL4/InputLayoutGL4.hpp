//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SRC_GL4_INPUTLAYOUTGL4_F0AF2367_CC09_4F30_B3CD_E1B0A3BE5C22_HPP
#define POMDOG_SRC_GL4_INPUTLAYOUTGL4_F0AF2367_CC09_4F30_B3CD_E1B0A3BE5C22_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "OpenGLPrerequisites.hpp"
#include "TypesafeGL4.hpp"
#include "../RenderSystem/NativeInputLayout.hpp"
#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>
#include <Pomdog/Utility/detail/Tagged.hpp>
#include <Pomdog/Utility/Optional.hpp>
#include <vector>
#include <limits>
#include <memory>

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

struct InputElementGL4 {
	// Attribute location.
	GLuint StartSlot;

	// Input element offset.
	std::uint32_t ByteOffset;

	// Specifies the scalar data type.
	ScalarTypeGL4 ScalarType;

	// Must be 1, 2, 3, and 4.
	std::int8_t Components;
	
	bool IsInteger = false;
};

struct InputBindingGL4 {
	std::vector<InputElementGL4> InputElements;
	std::uint32_t InstanceFrequency;
};

class InputLayoutGL4: public NativeInputLayout {
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
