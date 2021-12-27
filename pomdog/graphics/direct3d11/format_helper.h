// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/direct3d11/prerequisites_direct3d11.hpp"
#include "pomdog/graphics/forward_declarations.hpp"

namespace pomdog::detail::direct3d11 {

D3D11_COMPARISON_FUNC ToComparisonFunction(ComparisonFunction compareFunction) noexcept;

} // namespace pomdog::detail::direct3d11
