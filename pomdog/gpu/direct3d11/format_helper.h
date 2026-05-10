// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/direct3d11/prerequisites_direct3d11.h"

namespace pomdog::gpu {
enum class ComparisonFunction : u8;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::direct3d11 {

/// Converts a ComparisonFunction value to the equivalent D3D11_COMPARISON_FUNC.
[[nodiscard]] D3D11_COMPARISON_FUNC
toComparisonFunction(ComparisonFunction compareFunction) noexcept;

} // namespace pomdog::gpu::detail::direct3d11
