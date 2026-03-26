// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

/// Specifies how a texture resource can be used in the graphics pipeline.
enum class TextureUsage : u8 {
    /// The texture can be sampled from a shader.
    Sampled = 1 << 0,

    /// The texture can be used as a render target (color attachment).
    RenderTarget = 1 << 1,

    /// The texture can be used as a depth-stencil buffer.
    DepthStencil = 1 << 2,
};

[[nodiscard]] inline constexpr TextureUsage
operator|(TextureUsage a, TextureUsage b) noexcept
{
    return static_cast<TextureUsage>(static_cast<u8>(a) | static_cast<u8>(b));
}

[[nodiscard]] inline constexpr TextureUsage
operator&(TextureUsage a, TextureUsage b) noexcept
{
    return static_cast<TextureUsage>(static_cast<u8>(a) & static_cast<u8>(b));
}

[[nodiscard]] inline constexpr bool
hasFlag(TextureUsage flags, TextureUsage test) noexcept
{
    return (static_cast<u8>(flags) & static_cast<u8>(test)) != 0;
}

} // namespace pomdog::gpu
