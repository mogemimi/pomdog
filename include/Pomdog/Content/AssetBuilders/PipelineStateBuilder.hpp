// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Content/AssetBuilders/Builder.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>
#include <string>
#include <vector>

namespace Pomdog {
namespace Detail {
class AssetLoaderContext;
} // namespace Detail

namespace AssetBuilders {

template <>
class POMDOG_EXPORT Builder<PipelineState> final {
public:
    explicit Builder(const Detail::AssetLoaderContext& loaderContext);

    Builder(Builder&&);

    Builder& operator=(Builder&&);

    ~Builder();

    Builder& SetVertexShader(const std::shared_ptr<Shader>& vertexShader);

    Builder& SetVertexShader(std::shared_ptr<Shader>&& vertexShader);

    Builder& SetPixelShader(const std::shared_ptr<Shader>& pixelShader);

    Builder& SetPixelShader(std::shared_ptr<Shader>&& pixelShader);

    Builder& SetInputLayout(const InputLayoutDescription& inputLayout);

    Builder& SetInputLayout(InputLayoutDescription&& inputLayout);

    Builder& SetBlendState(const BlendDescription& blendState);

    Builder& SetRasterizerState(const RasterizerDescription& rasterizerState);

    Builder& SetDepthStencilState(const DepthStencilDescription& depthStencilState);

    Builder& SetConstantBufferBindSlot(const std::string& name, int slotIndex);

    Builder& SetRenderTargetViewFormat(SurfaceFormat renderTargetViewFormat);

    Builder& SetRenderTargetViewFormats(const std::vector<SurfaceFormat>& renderTargetViewFormats);

    Builder& SetRenderTargetViewFormats(std::vector<SurfaceFormat>&& renderTargetViewFormats);

    Builder& SetDepthStencilViewFormat(DepthFormat depthStencilViewFormat);

    std::shared_ptr<PipelineState> Build();

    std::shared_ptr<EffectReflection> CreateEffectReflection(
        const std::shared_ptr<PipelineState>& pipelineState);

    const PipelineStateDescription& GetDescription() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace AssetBuilders
} // namespace Pomdog
