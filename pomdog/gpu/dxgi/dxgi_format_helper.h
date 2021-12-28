// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <dxgi.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::dxgi {

[[nodiscard]] DXGI_FORMAT
ToDXGIFormat(SurfaceFormat format) noexcept;

[[nodiscard]] DXGI_FORMAT
ToDXGIFormat(IndexElementSize elementSize) noexcept;

[[nodiscard]] DXGI_FORMAT
ToDXGIFormat(InputElementFormat format) noexcept;

} // namespace pomdog::detail::dxgi
