// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/direct3d11/prerequisites_direct3d11.hpp"
#include "pomdog/graphics/forward_declarations.hpp"

namespace Pomdog::Detail::Direct3D11 {

D3D11_COMPARISON_FUNC ToComparisonFunction(ComparisonFunction compareFunction) noexcept;

} // namespace Pomdog::Detail::Direct3D11
