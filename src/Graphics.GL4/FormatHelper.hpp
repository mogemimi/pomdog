// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"

namespace Pomdog::Detail::GL4 {

GLenum ToComparisonFunctionGL4NonTypesafe(ComparisonFunction comparison) noexcept;

} // namespace Pomdog::Detail::GL4
