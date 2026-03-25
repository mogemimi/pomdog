// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

/// Specifies how a buffer can be bound to the graphics pipeline.
enum class BufferBindFlags : u8 {
    None = 0,
    VertexBuffer = 1 << 0,
    IndexBuffer = 1 << 1,
    ConstantBuffer = 1 << 2,
};

[[nodiscard]] inline constexpr BufferBindFlags
operator|(BufferBindFlags a, BufferBindFlags b) noexcept
{
    return static_cast<BufferBindFlags>(static_cast<u8>(a) | static_cast<u8>(b));
}

[[nodiscard]] inline constexpr BufferBindFlags
operator&(BufferBindFlags a, BufferBindFlags b) noexcept
{
    return static_cast<BufferBindFlags>(static_cast<u8>(a) & static_cast<u8>(b));
}

[[nodiscard]] inline constexpr bool
hasFlag(BufferBindFlags flags, BufferBindFlags test) noexcept
{
    return (static_cast<u8>(flags) & static_cast<u8>(test)) != 0;
}

} // namespace pomdog::gpu
