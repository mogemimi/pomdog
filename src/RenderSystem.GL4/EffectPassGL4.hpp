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

#include <cstdint>
#include <vector>
#include "OpenGLPrerequisites.hpp"
#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>
#include <Pomdog/Utility/Optional.hpp>
#include "../RenderSystem/NativeEffectPass.hpp"
#include "TypesafeGL4.hpp"

namespace Pomdog {
namespace Details {

class ShaderBytecode;

namespace RenderSystem {
namespace GL4 {

class ConstantBufferGL4;

struct ConstantBufferBindingGL4 {
	std::shared_ptr<ConstantBufferGL4> ConstantBuffer;
	std::string Name;
	std::uint32_t SlotIndex;
};

struct TextureBindingGL4 {
	GLint UniformLocation;
	std::uint32_t SlotIndex;
};

class EffectPassGL4 final: public NativeEffectPass {
public:
	EffectPassGL4() = delete;
	
	EffectPassGL4(ShaderBytecode const& vertexShaderBytecode, ShaderBytecode const& pixelShaderBytecode);
	
	~EffectPassGL4();
	
	void SetConstant(std::string const& constantName, std::shared_ptr<NativeConstantBuffer> const& constantBuffer) override;
	
	void SetConstant(std::string const& constantName) override;

	void Apply(GraphicsContext & graphicsContext,
		std::shared_ptr<EffectPass> const& sharedThisEffectPass) override;
	
	void ApplyShaders();
	
	ShaderProgramGL4 GetShaderProgram() const;
	
private:
	std::vector<ConstantBufferBindingGL4> constantBufferBindings;
	std::vector<TextureBindingGL4> textureBindings;
	Optional<ShaderProgramGL4> shaderProgram;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_GL4_EFFECTPASSGL4_5C5F046F_F42A_4AAD_AC22_389698C435EB_HPP)
