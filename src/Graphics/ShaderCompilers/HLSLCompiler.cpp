//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Pomdog/Graphics/ShaderCompilers/HLSLCompiler.hpp"
#include "Pomdog/Utility/Exception.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
std::unique_ptr<Shader> HLSLCompiler::CreateVertexShader(
	GraphicsDevice & graphicsDevice,
	void const* source, std::size_t byteLength, std::string const& entrypoint)
{
	///@todo Not implemented
	POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}
//-----------------------------------------------------------------------
std::unique_ptr<Shader> HLSLCompiler::CreatePixelShader(
	GraphicsDevice & graphicsDevice,
	void const* source, std::size_t byteLength, std::string const& entrypoint)
{
	///@todo Not implemented
	POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}
//-----------------------------------------------------------------------
}// namespace Pomdog
