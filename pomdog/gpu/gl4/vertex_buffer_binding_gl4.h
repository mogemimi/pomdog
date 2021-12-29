// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {

struct VertexBufferBindingGL4 final {
    std::shared_ptr<pomdog::VertexBuffer> VertexBuffer;
    std::size_t VertexOffset = 0;
};

} // namespace pomdog::gpu::detail::gl4
