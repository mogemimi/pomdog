// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <dxgi.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::dxgi {

[[nodiscard]] DXGI_FORMAT
toDXGIFormat(PixelFormat format) noexcept;

[[nodiscard]] DXGI_FORMAT
toDXGIFormat(IndexFormat elementSize) noexcept;

[[nodiscard]] DXGI_FORMAT
toDXGIFormat(InputElementFormat format) noexcept;

} // namespace pomdog::gpu::detail::dxgi
