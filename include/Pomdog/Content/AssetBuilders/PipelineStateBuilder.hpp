// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Content/AssetBuilders/Builder.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include <memory>
#include <string>
#include <vector>

namespace Pomdog {
class AssetManager;
class Error;
} // namespace Pomdog

namespace Pomdog::AssetBuilders {

template <>
class POMDOG_EXPORT Builder<PipelineState> final {
public:
    explicit Builder(AssetManager& assets);

    Builder(Builder&&);

    Builder& operator=(Builder&&);

    ~Builder();

    Builder& SetVertexShader(const std::shared_ptr<Shader>& vertexShader);

    Builder& SetVertexShader(std::shared_ptr<Shader>&& vertexShader);

    Builder& SetPixelShader(const std::shared_ptr<Shader>& pixelShader);

    Builder& SetPixelShader(std::shared_ptr<Shader>&& pixelShader);

    Builder& SetInputLayout(const InputLayoutDescription& inputLayout);

    Builder& SetInputLayout(InputLayoutDescription&& inputLayout);

    /// Specifies the geometric primitive type for the input assembler stage.
    Builder& SetPrimitiveTopology(PrimitiveTopology primitiveTopology);

    Builder& SetBlendState(const BlendDescription& blendState);

    Builder& SetRasterizerState(const RasterizerDescription& rasterizerState);

    Builder& SetDepthStencilState(const DepthStencilDescription& depthStencilState);

    Builder& SetConstantBufferBindSlot(const std::string& name, int slotIndex);

    Builder& SetSamplerBindSlot(const std::string& name, int slotIndex);

    Builder& SetRenderTargetViewFormat(SurfaceFormat renderTargetViewFormat);

    Builder& SetRenderTargetViewFormats(const std::vector<SurfaceFormat>& renderTargetViewFormats);

    Builder& SetRenderTargetViewFormats(std::vector<SurfaceFormat>&& renderTargetViewFormats);

    Builder& SetDepthStencilViewFormat(DepthFormat depthStencilViewFormat);

    /// Returns a pipeline state object.
    [[nodiscard]] std::tuple<std::shared_ptr<PipelineState>, std::shared_ptr<Error>>
    Build();

    [[nodiscard]] std::shared_ptr<EffectReflection>
    CreateEffectReflection(const std::shared_ptr<PipelineState>& pipelineState);

    const PipelineStateDescription& GetDescription() const;

    void SetError(std::shared_ptr<Error>&& err);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog::AssetBuilders
