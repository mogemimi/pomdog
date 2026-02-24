// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu::detail {

enum class BufferBindMode : u8 {
    ConstantBuffer,
    IndexBuffer,
    VertexBuffer,
};

} // namespace pomdog::gpu::detail
