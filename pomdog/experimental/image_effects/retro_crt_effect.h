// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/experimental/image_effects/image_effect_base.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
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

/// Applies a retro CRT monitor post-processing effect with scanlines.
class POMDOG_EXPORT RetroCrtEffect : public ImageEffectBase {
private:
    std::shared_ptr<gpu::SamplerState> samplerState_;
    std::shared_ptr<gpu::PipelineState> pipelineState_;

public:
    /// Initializes the effect by loading shaders and creating GPU resources.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<vfs::FileSystemContext>& fs,
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice);

    /// Updates GPU resources before rendering.
    void updateGPUResources() override;

    /// Applies the retro CRT effect to the source render target.
    void apply(
        gpu::CommandList& commandList,
        const std::shared_ptr<gpu::RenderTarget2D>& source,
        const std::shared_ptr<gpu::ConstantBuffer>& constantBuffer) override;
};

} // namespace pomdog
