// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"

namespace pomdog::gpu {
enum class ComparisonFunction : u8;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::gl4 {

[[nodiscard]] GLenum
toComparisonFunctionGL4NonTypesafe(ComparisonFunction comparison) noexcept;

} // namespace pomdog::gpu::detail::gl4
