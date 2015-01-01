//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SHADERCOMPILING_C56F5C9E_7275_478F_989F_35B4404DC08F_HPP
#define POMDOG_SHADERCOMPILING_C56F5C9E_7275_478F_989F_35B4404DC08F_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "PrerequisitesDirect3D11.hpp"
#include <string>
#include <vector>
#include <cstdint>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace Direct3D11 {

enum class ShaderProgramType: std::uint8_t {
	VertexShader,
	PixelShader,
	GeometryShader,
	DomainShader,
	ComputeShader,
	HullShader,
};

enum class ShaderModel: std::uint8_t {
	SM_2_0,
	SM_3_0,
	SM_4_0,
	SM_5_0,
};

struct ShaderProfile {
	ShaderModel ShaderModel;
	ShaderProgramType ProgramType;
};

class ShaderCompiling final {
public:
	static std::vector<std::uint8_t> CompileShader(std::string const& source,
		ShaderProfile const& shaderProfile, std::string const& entrypoint,
		std::string const& currentDirectory);

	//static std::vector<std::uint8_t> CompileShader(std::string const& source,
	//	ShaderProfile const& shaderProfile, std::string const& entrypoint,
	//	std::vector<D3D_SHADER_MACRO> const& defines,
	//	std::string const& currentDirectory);
};

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SHADERCOMPILING_C56F5C9E_7275_478F_989F_35B4404DC08F_HPP)
