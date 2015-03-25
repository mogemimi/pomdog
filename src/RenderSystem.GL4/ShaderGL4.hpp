// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SHADERGL4_39E4A790_D7E4_4CA7_960D_F27CF80FCF2A_HPP
#define POMDOG_SHADERGL4_39E4A790_D7E4_4CA7_960D_F27CF80FCF2A_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "OpenGLPrerequisites.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Utility/Optional.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class ShaderBytecode;

namespace GL4 {

template <GLenum PipelineStage>
class ShaderGL4 final: public Shader {
public:
    explicit ShaderGL4(ShaderBytecode const& source);

    ~ShaderGL4();

    GLuint NativeShader() const;

private:
    static constexpr GLenum pipelineStage = PipelineStage;
    static_assert(pipelineStage == GL_VERTEX_SHADER
        || pipelineStage == GL_FRAGMENT_SHADER
        || pipelineStage == GL_GEOMETRY_SHADER
        || pipelineStage == GL_TESS_CONTROL_SHADER
        || pipelineStage == GL_TESS_EVALUATION_SHADER
    #ifdef GL_COMPUTE_SHADER
        || pipelineStage == GL_COMPUTE_SHADER
    #endif
        , "");

    Optional<GLuint> shader;
};

using VertexShaderGL4 = ShaderGL4<GL_VERTEX_SHADER>;
using PixelShaderGL4 = ShaderGL4<GL_FRAGMENT_SHADER>;

}// namespace GL4
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_SHADERGL4_39E4A790_D7E4_4CA7_960D_F27CF80FCF2A_HPP)
