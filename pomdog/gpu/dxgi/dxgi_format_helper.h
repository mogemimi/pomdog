// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <dxgi.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
enum class PixelFormat : u8;
enum class IndexFormat : u8;
enum class InputElementFormat : u8;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::dxgi {

/// Converts a PixelFormat to the equivalent DXGI_FORMAT.
[[nodiscard]] DXGI_FORMAT
toDXGIFormat(PixelFormat format) noexcept;

/// Converts an IndexFormat to the equivalent DXGI_FORMAT.
[[nodiscard]] DXGI_FORMAT
toDXGIFormat(IndexFormat elementSize) noexcept;

/// Converts an InputElementFormat to the equivalent DXGI_FORMAT.
[[nodiscard]] DXGI_FORMAT
toDXGIFormat(InputElementFormat format) noexcept;

} // namespace pomdog::gpu::detail::dxgi
