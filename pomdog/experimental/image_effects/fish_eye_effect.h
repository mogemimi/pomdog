// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/experimental/image_effects/image_effect_base.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class ConstantBuffer;
class GraphicsDevice;
class PipelineState;
class SamplerState;
} // namespace pomdog::gpu

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog {

class POMDOG_EXPORT FishEyeEffect final : public ImageEffectBase {
private:
    std::shared_ptr<gpu::SamplerState> samplerLinear_;
    std::shared_ptr<gpu::PipelineState> pipelineState_;
    std::shared_ptr<gpu::ConstantBuffer> constantBufferFishEye_;
    float strength_;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<vfs::FileSystemContext>& fs,
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice);

    void setStrength(float strength);

    void updateGPUResources() override;

    void apply(
        gpu::CommandList& commandList,
        const std::shared_ptr<gpu::RenderTarget2D>& source,
        const std::shared_ptr<gpu::ConstantBuffer>& constantBuffer) override;
};

} // namespace pomdog
