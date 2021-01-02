// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "BlendStateGL4.hpp"
#include "DepthStencilStateGL4.hpp"
#include "OpenGLPrerequisites.hpp"
#include "RasterizerStateGL4.hpp"
#include "TypesafeGL4.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/PipelineState.hpp"
#include "Pomdog/Utility/Errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::GL4 {

class InputLayoutGL4;
using PrimitiveTopologyGL4 = Tagged<GLenum, PrimitiveTopology>;

struct TextureBindingGL4 final {
    GLint UniformLocation;
    std::uint16_t SlotIndex;
};

class PipelineStateGL4 final : public PipelineState {
public:
    PipelineStateGL4();

    ~PipelineStateGL4() override;

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(const PipelineStateDescription& description) noexcept;

    void ApplyShaders();

    ShaderProgramGL4 GetShaderProgram() const noexcept;

    InputLayoutGL4* GetInputLayout() const noexcept;

    PrimitiveTopologyGL4 GetPrimitiveTopology() const noexcept;

private:
    std::vector<TextureBindingGL4> textureBindings;
    BlendStateGL4 blendState;
    RasterizerStateGL4 rasterizerState;
    DepthStencilStateGL4 depthStencilState;
    std::optional<ShaderProgramGL4> shaderProgram;
    std::unique_ptr<InputLayoutGL4> inputLayout;
    PrimitiveTopologyGL4 primitiveTopology;
};

} // namespace Pomdog::Detail::GL4
