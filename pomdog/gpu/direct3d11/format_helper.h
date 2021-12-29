// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/direct3d11/prerequisites_direct3d11.h"
#include "pomdog/gpu/forward_declarations.h"

namespace pomdog::gpu::detail::direct3d11 {

D3D11_COMPARISON_FUNC ToComparisonFunction(ComparisonFunction compareFunction) noexcept;

} // namespace pomdog::gpu::detail::direct3d11
