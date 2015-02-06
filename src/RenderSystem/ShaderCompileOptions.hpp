//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SHADERCOMPILEOPTIONS_11A9FF1A_9BA1_431B_AE9E_BB99CD2AB5F9_HPP
#define POMDOG_SHADERCOMPILEOPTIONS_11A9FF1A_9BA1_431B_AE9E_BB99CD2AB5F9_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <cstdint>
#include <tuple>
#include <string>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

enum class ShaderPipelineStage: std::uint8_t {
	VertexShader,
	PixelShader,
	//GeometryShader,
	//DomainShader,
	//ComputeShader,
	//HullShader,
};

class ShaderModel {
public:
	std::uint8_t Major = 4;
	std::uint8_t Minor = 0;
};

class ShaderProfile {
public:
	ShaderModel ShaderModel;
	ShaderPipelineStage PipelineStage;
};

class ShaderCompileOptions {
public:
	std::string EntryPoint;
	std::vector<std::tuple<std::string, std::string>> PreprocessorMacros;
	ShaderProfile Profile;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SHADERCOMPILEOPTIONS_11A9FF1A_9BA1_431B_AE9E_BB99CD2AB5F9_HPP)
