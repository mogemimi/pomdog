// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class VertexBuffer;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::gl4 {

/// VertexBufferBindingGL4 pairs a VertexBuffer with its byte offset into that buffer.
struct VertexBufferBindingGL4 final {
    std::shared_ptr<pomdog::gpu::VertexBuffer> vertexBuffer;
    u32 vertexOffset = 0;
};

} // namespace pomdog::gpu::detail::gl4
