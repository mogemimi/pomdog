// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PIPELINESTATEGL4_5C5F046F_HPP
#define POMDOG_PIPELINESTATEGL4_5C5F046F_HPP

#include "OpenGLPrerequisites.hpp"
#include "BlendStateGL4.hpp"
#include "DepthStencilStateGL4.hpp"
#include "RasterizerStateGL4.hpp"
#include "TypesafeGL4.hpp"
#include "../RenderSystem/NativePipelineState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <cstdint>
#include <memory>
#include <string>
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

class PipelineStateGL4 final : public NativePipelineState {
public:
    explicit PipelineStateGL4(PipelineStateDescription const& description);

    ~PipelineStateGL4();

    std::unique_ptr<NativeConstantLayout> CreateConstantLayout() override;

    void ApplyShaders();

    ShaderProgramGL4 GetShaderProgram() const;

    InputLayoutGL4* GetInputLayout() const;

private:
    std::vector<TextureBindingGL4> textureBindings;
    std::vector<UniformBlockBindingGL4> uniformBlockBindings;
    BlendStateGL4 blendState;
    RasterizerStateGL4 rasterizerState;
    DepthStencilStateGL4 depthStencilState;
    Optional<ShaderProgramGL4> shaderProgram;
    std::unique_ptr<InputLayoutGL4> inputLayout;
};

} // namespace GL4
} // namespace RenderSystem
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_PIPELINESTATEGL4_5C5F046F_HPP
