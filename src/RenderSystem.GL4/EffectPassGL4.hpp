//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_GL4_EFFECTPASSGL4_5C5F046F_F42A_4AAD_AC22_389698C435EB_HPP
#define POMDOG_SRC_GL4_EFFECTPASSGL4_5C5F046F_F42A_4AAD_AC22_389698C435EB_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "OpenGLPrerequisites.hpp"
#include "TypesafeGL4.hpp"
#include "../RenderSystem/NativeEffectPass.hpp"
#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>
#include <Pomdog/Utility/Optional.hpp>
#include <cstdint>
#include <vector>

namespace Pomdog {
namespace Details {

class ShaderBytecode;

namespace RenderSystem {
namespace GL4 {

struct TextureBindingGL4 {
	GLint UniformLocation;
	std::uint16_t SlotIndex;
};

struct UniformBlockBindingGL4 {
	std::string Name;
	std::uint16_t SlotIndex;
};

class EffectPassGL4 final: public NativeEffectPass {
public:
	EffectPassGL4() = delete;
	
	EffectPassGL4(ShaderBytecode const& vertexShaderBytecode, ShaderBytecode const& pixelShaderBytecode);
	
	~EffectPassGL4();

	std::unique_ptr<NativeConstantLayout> CreateConstantLayout() override;

	void ApplyShaders();
	
	ShaderProgramGL4 GetShaderProgram() const;

private:
	std::vector<TextureBindingGL4> textureBindings;
	std::vector<UniformBlockBindingGL4> uniformBlockBindings;
	Optional<ShaderProgramGL4> shaderProgram;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_GL4_EFFECTPASSGL4_5C5F046F_F42A_4AAD_AC22_389698C435EB_HPP)
