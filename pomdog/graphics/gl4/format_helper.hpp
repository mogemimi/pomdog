// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/graphics/gl4/opengl_prerequisites.hpp"

namespace pomdog::detail::gl4 {

GLenum ToComparisonFunctionGL4NonTypesafe(ComparisonFunction comparison) noexcept;

} // namespace pomdog::detail::gl4
