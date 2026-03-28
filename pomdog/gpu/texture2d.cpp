// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/texture2d.h"

#include "pomdog/gpu/texture_usage.h"

namespace pomdog::gpu {

Texture2D::Texture2D() noexcept = default;
Texture2D::~Texture2D() = default;

TextureUsage Texture2D::getUsage() const noexcept
{
    return TextureUsage::Sampled;
}

} // namespace pomdog::gpu
