// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class CommandList;
class ConstantBuffer;
class RenderTarget2D;
} // namespace pomdog::gpu

namespace pomdog {

/// Interface for image effects that require pre-rendering passes.
class POMDOG_EXPORT ImageEffectPreRenderable {
public:
    virtual ~ImageEffectPreRenderable();

    /// Performs a pre-render pass before the main post-processing chain.
    virtual void preRender(
        gpu::CommandList& commandList,
        const std::shared_ptr<gpu::ConstantBuffer>& constantBuffer,
        const std::function<void()>& draw) = 0;
};

/// Base class for all post-processing image effects.
class ImageEffectBase {
public:
    virtual ~ImageEffectBase();

    /// Updates GPU resources before rendering.
    virtual void updateGPUResources() = 0;

    /// Applies the image effect to the given source render target.
    virtual void apply(
        gpu::CommandList& commandList,
        const std::shared_ptr<gpu::RenderTarget2D>& source,
        const std::shared_ptr<gpu::ConstantBuffer>& constantBuffer) = 0;
};

} // namespace pomdog
