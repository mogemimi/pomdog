//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Pomdog/Graphics/ShaderCompilers/GLSLCompiler.hpp"
#include "Pomdog/Utility/Exception.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
std::unique_ptr<Shader> GLSLCompiler::CreateVertexShader(
	GraphicsDevice & graphicsDevice, void const* shaderBytecode, std::size_t byteLength)
{
	///@todo Not implemented
	POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}
//-----------------------------------------------------------------------
std::unique_ptr<Shader> GLSLCompiler::CreatePixelShader(
	GraphicsDevice & graphicsDevice, void const* shaderBytecode, std::size_t byteLength)
{
	///@todo Not implemented
	POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}
//-----------------------------------------------------------------------
}// namespace Pomdog
