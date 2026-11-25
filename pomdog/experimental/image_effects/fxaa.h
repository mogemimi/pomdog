// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
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

/// Applies Fast Approximate Anti-Aliasing (FXAA) as a post-processing effect.
///
/// The effect is Pomdog's own filter based on the ideas of FXAA in a single
/// pass. It searches along each edge for the ends of its stair steps, so it
/// also smooths long edges that are nearly horizontal or vertical. It keeps
/// the brightness of lines one pixel wide, keeps the corners of rectangles and
/// pixel-aligned shapes sharp, and leaves pixels that are already
/// anti-aliased, such as text, as they are. Pixels with low local contrast
/// skip the search, which keeps flat areas cheap. The source should hold
/// gamma-encoded colors, as an 8-bit back buffer format does.
class POMDOG_EXPORT FXAA final : public ImageEffectBase {
private:
    std::shared_ptr<gpu::SamplerState> samplerLinear_;
    std::shared_ptr<gpu::PipelineState> pipelineState_;
    std::shared_ptr<gpu::ConstantBuffer> constantBufferFXAA_;
    f32 edgeThreshold_ = 0.166f;
    f32 edgeThresholdMin_ = 0.0833f;

public:
    /// Initializes the effect by loading shaders and creating GPU resources.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<vfs::FileSystemContext>& fs,
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice);

    /// Sets the local contrast that a pixel needs, relative to its brightest
    /// neighbor, to be treated as an edge, in [0, 1]. Lower values smooth
    /// fainter edges and cost more. The default is 0.166.
    void setEdgeThreshold(f32 threshold) noexcept;

    /// Sets the local contrast that a pixel needs to be treated as an edge,
    /// in [0, 1]. It skips dark areas, where the relative threshold alone
    /// would treat noise as edges. The default is 0.0833.
    void setEdgeThresholdMin(f32 threshold) noexcept;

    /// Updates GPU resources before rendering.
    void updateGPUResources() override;

    /// Applies the FXAA effect to the source render target.
    void apply(
        gpu::CommandList& commandList,
        const std::shared_ptr<gpu::RenderTarget2D>& source,
        const std::shared_ptr<gpu::ConstantBuffer>& constantBuffer) override;
};

} // namespace pomdog
