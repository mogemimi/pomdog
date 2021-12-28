// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/graphics/forward_declarations.h"
#include "pomdog/graphics/gl4/blend_state_gl4.h"
#include "pomdog/graphics/gl4/depth_stencil_state_gl4.h"
#include "pomdog/graphics/gl4/opengl_prerequisites.h"
#include "pomdog/graphics/gl4/rasterizer_state_gl4.h"
#include "pomdog/graphics/gl4/typesafe_gl4.h"
#include "pomdog/graphics/pipeline_state.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::gl4 {

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

} // namespace pomdog::detail::gl4
