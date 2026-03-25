// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/index_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class Buffer;
} // namespace pomdog::gpu

namespace pomdog::gpu {

/// A lightweight view of a Buffer for use as a vertex buffer.
struct VertexBufferView final {
    std::shared_ptr<Buffer> buffer;
    u32 strideInBytes = 0;
    u32 offset = 0;
};

/// A lightweight view of a Buffer for use as an index buffer.
struct IndexBufferView final {
    std::shared_ptr<Buffer> buffer;
    IndexFormat format = IndexFormat::UInt16;
    u32 offset = 0;
};

/// A lightweight view of a Buffer for use as a constant (uniform) buffer.
struct ConstantBufferView final {
    std::shared_ptr<Buffer> buffer;
    u32 sizeInBytes = 0;
    u32 offset = 0;
};

} // namespace pomdog::gpu
