// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/gl4/blend_state_gl4.h"
#include "pomdog/gpu/gl4/depth_stencil_state_gl4.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/gpu/gl4/rasterizer_state_gl4.h"
#include "pomdog/gpu/gl4/typesafe_gl4.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/memory/unsafe_ptr.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
struct PipelineDesc;
enum class PrimitiveTopology : u8;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::gl4 {
class InputLayoutGL4;
} // namespace pomdog::gpu::detail::gl4

namespace pomdog::gpu::detail::gl4 {

using PrimitiveTopologyGL4 = pomdog::detail::Tagged<GLenum, PrimitiveTopology>;

struct TextureBindingGL4 final {
    GLint uniformLocation;
    u16 slotIndex;
};

class PipelineStateGL4 final : public PipelineState {
private:
    std::vector<TextureBindingGL4> textureBindings_ = {};
    BlendStateGL4 blendState_ = {};
    RasterizerStateGL4 rasterizerState_ = {};
    DepthStencilStateGL4 depthStencilState_ = {};
    std::optional<ShaderProgramGL4> shaderProgram_ = std::nullopt;
    std::unique_ptr<InputLayoutGL4> inputLayout_;
    PrimitiveTopologyGL4 primitiveTopology_ = {};

public:
    PipelineStateGL4();

    ~PipelineStateGL4() override;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const PipelineDesc& descriptor) noexcept;

    void applyShaders();

    [[nodiscard]] ShaderProgramGL4
    getShaderProgram() const noexcept;

    [[nodiscard]] unsafe_ptr<InputLayoutGL4>
    getInputLayout() const noexcept;

    [[nodiscard]] PrimitiveTopologyGL4
    getPrimitiveTopology() const noexcept;
};

} // namespace pomdog::gpu::detail::gl4
