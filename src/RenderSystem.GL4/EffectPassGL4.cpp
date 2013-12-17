//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "EffectPassGL4.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {
//-----------------------------------------------------------------------
class InputLayoutGL4
{
public:

};

namespace {
namespace Tags {

struct VertexShaderTag {};
struct PixelShaderTag {};

}// namespace Tags

using VertexShaderGL4 = Tagged<GLuint, Tags::VertexShaderTag>;
using PixelShaderGL4 = Tagged<GLuint, Tags::PixelShaderTag>;

}// namespace

//-----------------------------------------------------------------------
EffectPassGL4::EffectPassGL4(ShaderBytecode const& vertexShaderBytecode,
	ShaderBytecode const& pixelShaderBytecode)
{
}
//-----------------------------------------------------------------------
EffectPassGL4::~EffectPassGL4()
{
	if (shaderProgram) {
		glDeleteProgram((*shaderProgram).value);
	}
}
//-----------------------------------------------------------------------
void EffectPassGL4::ApplyShaders()
{
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
