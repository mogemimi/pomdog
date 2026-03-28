// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/graphics_device.h"

namespace pomdog::gpu {

GraphicsDevice::GraphicsDevice() noexcept = default;
GraphicsDevice::~GraphicsDevice() = default;

bool GraphicsDevice::isRenderTargetTextureFlipped() const noexcept
{
    return false;
}

} // namespace pomdog::gpu
