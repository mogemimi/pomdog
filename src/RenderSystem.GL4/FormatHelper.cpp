// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "FormatHelper.hpp"
#include "../Basic/Unreachable.hpp"
#include "Pomdog/Graphics/ComparisonFunction.hpp"

namespace Pomdog::Detail::GL4 {

GLenum ToComparisonFunctionGL4NonTypesafe(ComparisonFunction comparison) noexcept
{
    switch (comparison) {
    case ComparisonFunction::Always:
        return GL_ALWAYS;
    case ComparisonFunction::Equal:
        return GL_EQUAL;
    case ComparisonFunction::Greater:
        return GL_GREATER;
    case ComparisonFunction::GreaterEqual:
        return GL_GEQUAL;
    case ComparisonFunction::Less:
        return GL_LESS;
    case ComparisonFunction::LessEqual:
        return GL_LEQUAL;
    case ComparisonFunction::Never:
        return GL_NEVER;
    case ComparisonFunction::NotEqual:
        return GL_NOTEQUAL;
    }
    POMDOG_UNREACHABLE("Unsupported comparison function");
}

} // namespace Pomdog::Detail::GL4
