// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "FormatHelper.hpp"
#include "Pomdog/Graphics/ComparisonFunction.hpp"

namespace Pomdog::Detail::Direct3D11 {

D3D11_COMPARISON_FUNC ToComparisonFunction(ComparisonFunction compareFunction) noexcept
{
    switch (compareFunction) {
    case ComparisonFunction::Never:
        return D3D11_COMPARISON_NEVER;
    case ComparisonFunction::Less:
        return D3D11_COMPARISON_LESS;
    case ComparisonFunction::Equal:
        return D3D11_COMPARISON_EQUAL;
    case ComparisonFunction::LessEqual:
        return D3D11_COMPARISON_LESS_EQUAL;
    case ComparisonFunction::Greater:
        return D3D11_COMPARISON_GREATER;
    case ComparisonFunction::NotEqual:
        return D3D11_COMPARISON_NOT_EQUAL;
    case ComparisonFunction::GreaterEqual:
        return D3D11_COMPARISON_GREATER_EQUAL;
    case ComparisonFunction::Always:
        return D3D11_COMPARISON_ALWAYS;
    }
    return D3D11_COMPARISON_LESS_EQUAL;
}

} // namespace Pomdog::Detail::Direct3D11
