// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/render_target2d.h"

#include "pomdog/gpu/texture_usage.h"

namespace pomdog::gpu {

RenderTarget2D::RenderTarget2D() noexcept = default;
RenderTarget2D::~RenderTarget2D() = default;

TextureUsage RenderTarget2D::getUsage() const noexcept
{
    return TextureUsage::RenderTarget;
}

} // namespace pomdog::gpu
