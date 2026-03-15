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

class POMDOG_EXPORT ImageEffectPreRenderable {
public:
    virtual ~ImageEffectPreRenderable();

    virtual void preRender(
        gpu::CommandList& commandList,
        const std::shared_ptr<gpu::ConstantBuffer>& constantBuffer,
        const std::function<void()>& draw) = 0;
};

class ImageEffectBase {
public:
    virtual ~ImageEffectBase();

    virtual void updateGPUResources() = 0;

    virtual void apply(
        gpu::CommandList& commandList,
        const std::shared_ptr<gpu::RenderTarget2D>& source,
        const std::shared_ptr<gpu::ConstantBuffer>& constantBuffer) = 0;
};

} // namespace pomdog
