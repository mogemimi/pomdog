// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class GraphicsDevice;
class PipelineState;
class Shader;
struct BlendDesc;
struct DepthStencilDesc;
struct InputLayoutDesc;
struct PipelineDesc;
struct RasterizerDesc;
enum class PixelFormat : u8;
enum class PrimitiveTopology : u8;
} // namespace pomdog::gpu

namespace pomdog {

class POMDOG_EXPORT PipelineStateBuilder final {
public:
    explicit PipelineStateBuilder(const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice);

    PipelineStateBuilder(PipelineStateBuilder&&);

    PipelineStateBuilder& operator=(PipelineStateBuilder&&);

    ~PipelineStateBuilder();

    void setVertexShader(const std::shared_ptr<gpu::Shader>& vertexShader);

    void setVertexShader(std::shared_ptr<gpu::Shader>&& vertexShader);

    void setPixelShader(const std::shared_ptr<gpu::Shader>& pixelShader);

    void setPixelShader(std::shared_ptr<gpu::Shader>&& pixelShader);

    void setInputLayout(const gpu::InputLayoutDesc& inputLayout);

    void setInputLayout(gpu::InputLayoutDesc&& inputLayout);

    /// Specifies the geometric primitive type for the input assembler stage.
    void setPrimitiveTopology(gpu::PrimitiveTopology primitiveTopology);

    void setBlendState(const gpu::BlendDesc& blendState);

    void setRasterizerState(const gpu::RasterizerDesc& rasterizerState);

    void setDepthStencilState(const gpu::DepthStencilDesc& depthStencilState);

    void setRenderTargetViewFormat(gpu::PixelFormat renderTargetViewFormat);

    void setRenderTargetViewFormats(const std::vector<gpu::PixelFormat>& renderTargetViewFormats);

    void setRenderTargetViewFormats(std::vector<gpu::PixelFormat>&& renderTargetViewFormats);

    void setDepthStencilViewFormat(gpu::PixelFormat depthStencilViewFormat);

    /// Returns a pipeline state object.
    [[nodiscard]] std::tuple<std::shared_ptr<gpu::PipelineState>, std::unique_ptr<Error>>
    build();

    [[nodiscard]] const gpu::PipelineDesc&
    getDescription() const;

    void setError(std::unique_ptr<Error>&& err);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace pomdog
