//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SHADERCOMPILING_C56F5C9E_7275_478F_989F_35B4404DC08F_HPP
#define POMDOG_SHADERCOMPILING_C56F5C9E_7275_478F_989F_35B4404DC08F_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "PrerequisitesDirect3D11.hpp"
#include <cstdint>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class ShaderBytecode;
class ShaderCompileOptions;

namespace Direct3D11 {

struct ShaderCompiling final {
	static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
		ShaderBytecode const& shaderBytecode,
		ShaderCompileOptions const& compileOptions);
};

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_SHADERCOMPILING_C56F5C9E_7275_478F_989F_35B4404DC08F_HPP)
