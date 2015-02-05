//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_HLSLCOMPILER_223CB751_DD53_4FAB_8C8B_BB901AE266BA_HPP
#define POMDOG_HLSLCOMPILER_223CB751_DD53_4FAB_8C8B_BB901AE266BA_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <memory>
#include <cstddef>
#include <string>

namespace Pomdog {

class GraphicsDevice;
class Shader;

struct HLSLCompiler final {
	static std::unique_ptr<Shader> CreateVertexShader(GraphicsDevice & graphicsDevice,
		void const* shaderBytecode, std::size_t byteLength,
		std::string const& entrypoint);

	static std::unique_ptr<Shader> CreatePixelShader(GraphicsDevice & graphicsDevice,
		void const* shaderBytecode, std::size_t byteLength,
		std::string const& entrypoint);
};

}// namespace Pomdog

#endif // !defined(POMDOG_HLSLCOMPILER_223CB751_DD53_4FAB_8C8B_BB901AE266BA_HPP)
