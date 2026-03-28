// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/depth_stencil_buffer.h"
#include "pomdog/gpu/texture_usage.h"

namespace pomdog::gpu {

DepthStencilBuffer::DepthStencilBuffer() noexcept = default;

DepthStencilBuffer::~DepthStencilBuffer() = default;

TextureUsage DepthStencilBuffer::getUsage() const noexcept
{
    return TextureUsage::DepthStencil;
}

} // namespace pomdog::gpu
