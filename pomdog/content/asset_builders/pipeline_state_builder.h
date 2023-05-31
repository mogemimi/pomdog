// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/content/asset_builders/builder.h"
#include "pomdog/gpu/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class AssetManager;
class Error;
} // namespace pomdog

namespace pomdog::AssetBuilders {

template <>
class POMDOG_EXPORT Builder<gpu::PipelineState> final {
public:
    explicit Builder(AssetManager& assets);

    Builder(Builder&&);

    Builder& operator=(Builder&&);

    ~Builder();

    Builder& SetVertexShader(const std::shared_ptr<gpu::Shader>& vertexShader);

    Builder& SetVertexShader(std::shared_ptr<gpu::Shader>&& vertexShader);

    Builder& SetPixelShader(const std::shared_ptr<gpu::Shader>& pixelShader);

    Builder& SetPixelShader(std::shared_ptr<gpu::Shader>&& pixelShader);

    Builder& SetInputLayout(const gpu::InputLayoutDescriptor& inputLayout);

    Builder& SetInputLayout(gpu::InputLayoutDescriptor&& inputLayout);

    /// Specifies the geometric primitive type for the input assembler stage.
    Builder& SetPrimitiveTopology(gpu::PrimitiveTopology primitiveTopology);

    Builder& SetBlendState(const gpu::BlendDescriptor& blendState);

    Builder& SetRasterizerState(const gpu::RasterizerDescriptor& rasterizerState);

    Builder& SetDepthStencilState(const gpu::DepthStencilDescriptor& depthStencilState);

    Builder& SetConstantBufferBindSlot(const std::string& name, int slotIndex);

    Builder& SetSamplerBindSlot(const std::string& name, int slotIndex);

    Builder& SetRenderTargetViewFormat(PixelFormat renderTargetViewFormat);

    Builder& SetRenderTargetViewFormats(const std::vector<PixelFormat>& renderTargetViewFormats);

    Builder& SetRenderTargetViewFormats(std::vector<PixelFormat>&& renderTargetViewFormats);

    Builder& SetDepthStencilViewFormat(PixelFormat depthStencilViewFormat);

    /// Returns a pipeline state object.
    [[nodiscard]] std::tuple<std::shared_ptr<gpu::PipelineState>, std::unique_ptr<Error>>
    Build();

    [[nodiscard]] std::shared_ptr<gpu::EffectReflection>
    createEffectReflection(const std::shared_ptr<gpu::PipelineState>& pipelineState);

    const gpu::PipelineDescriptor& GetDescription() const;

    void SetError(std::unique_ptr<Error>&& err);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace pomdog::AssetBuilders
