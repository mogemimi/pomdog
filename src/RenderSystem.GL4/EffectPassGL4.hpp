// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EFFECTPASSGL4_5C5F046F_F42A_4AAD_AC22_389698C435EB_HPP
#define POMDOG_EFFECTPASSGL4_5C5F046F_F42A_4AAD_AC22_389698C435EB_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "OpenGLPrerequisites.hpp"
#include "TypesafeGL4.hpp"
#include "../RenderSystem/NativeEffectPass.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <cstdint>
#include <vector>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace GL4 {

class InputLayoutGL4;

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

	explicit EffectPassGL4(EffectPassDescription const& description);

	~EffectPassGL4();

	std::unique_ptr<NativeConstantLayout> CreateConstantLayout() override;

	void ApplyShaders();

	ShaderProgramGL4 GetShaderProgram() const;

	InputLayoutGL4* GetInputLayout() const;

private:
	std::vector<TextureBindingGL4> textureBindings;
	std::vector<UniformBlockBindingGL4> uniformBlockBindings;
	Optional<ShaderProgramGL4> shaderProgram;
	std::unique_ptr<InputLayoutGL4> inputLayout;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTPASSGL4_5C5F046F_F42A_4AAD_AC22_389698C435EB_HPP)
