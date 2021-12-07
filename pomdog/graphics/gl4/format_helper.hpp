// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/graphics/gl4/opengl_prerequisites.hpp"

namespace Pomdog::Detail::GL4 {

GLenum ToComparisonFunctionGL4NonTypesafe(ComparisonFunction comparison) noexcept;

} // namespace Pomdog::Detail::GL4
