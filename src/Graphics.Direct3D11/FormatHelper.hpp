// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D11.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"

namespace Pomdog::Detail::Direct3D11 {

D3D11_COMPARISON_FUNC ToComparisonFunction(ComparisonFunction compareFunction) noexcept;

} // namespace Pomdog::Detail::Direct3D11
