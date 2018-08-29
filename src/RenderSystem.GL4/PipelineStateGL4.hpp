// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "BlendStateGL4.hpp"
#include "DepthStencilStateGL4.hpp"
#include "OpenGLPrerequisites.hpp"
#include "RasterizerStateGL4.hpp"
#include "TypesafeGL4.hpp"
#include "../RenderSystem/NativePipelineState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace Pomdog {
namespace Detail {
namespace GL4 {

class InputLayoutGL4;

struct TextureBindingGL4 {
    GLint UniformLocation;
    std::uint16_t SlotIndex;
};

class PipelineStateGL4 final : public NativePipelineState {
public:
    explicit PipelineStateGL4(const PipelineStateDescription& description);

    ~PipelineStateGL4();

    void ApplyShaders();

    ShaderProgramGL4 GetShaderProgram() const;

    InputLayoutGL4* GetInputLayout() const;

private:
    std::vector<TextureBindingGL4> textureBindings;
    BlendStateGL4 blendState;
    RasterizerStateGL4 rasterizerState;
    DepthStencilStateGL4 depthStencilState;
    std::optional<ShaderProgramGL4> shaderProgram;
    std::unique_ptr<InputLayoutGL4> inputLayout;
};

} // namespace GL4
} // namespace Detail
} // namespace Pomdog
