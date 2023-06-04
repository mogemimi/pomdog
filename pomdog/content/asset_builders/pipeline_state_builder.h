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

    Builder& setVertexShader(const std::shared_ptr<gpu::Shader>& vertexShader);

    Builder& setVertexShader(std::shared_ptr<gpu::Shader>&& vertexShader);

    Builder& setPixelShader(const std::shared_ptr<gpu::Shader>& pixelShader);

    Builder& setPixelShader(std::shared_ptr<gpu::Shader>&& pixelShader);

    Builder& setInputLayout(const gpu::InputLayoutDescriptor& inputLayout);

    Builder& setInputLayout(gpu::InputLayoutDescriptor&& inputLayout);

    /// Specifies the geometric primitive type for the input assembler stage.
    Builder& setPrimitiveTopology(gpu::PrimitiveTopology primitiveTopology);

    Builder& setBlendState(const gpu::BlendDescriptor& blendState);

    Builder& setRasterizerState(const gpu::RasterizerDescriptor& rasterizerState);

    Builder& setDepthStencilState(const gpu::DepthStencilDescriptor& depthStencilState);

    Builder& setConstantBufferBindSlot(const std::string& name, int slotIndex);

    Builder& setSamplerBindSlot(const std::string& name, int slotIndex);

    Builder& setRenderTargetViewFormat(PixelFormat renderTargetViewFormat);

    Builder& setRenderTargetViewFormats(const std::vector<PixelFormat>& renderTargetViewFormats);

    Builder& setRenderTargetViewFormats(std::vector<PixelFormat>&& renderTargetViewFormats);

    Builder& setDepthStencilViewFormat(PixelFormat depthStencilViewFormat);

    /// Returns a pipeline state object.
    [[nodiscard]] std::tuple<std::shared_ptr<gpu::PipelineState>, std::unique_ptr<Error>>
    build();

    [[nodiscard]] std::shared_ptr<gpu::EffectReflection>
    createEffectReflection(const std::shared_ptr<gpu::PipelineState>& pipelineState);

    [[nodiscard]] const gpu::PipelineDescriptor&
    getDescription() const;

    void setError(std::unique_ptr<Error>&& err);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace pomdog::AssetBuilders
