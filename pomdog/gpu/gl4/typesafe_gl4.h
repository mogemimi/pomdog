// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/utility/tagged.h"

namespace pomdog::gpu::detail::gl4 {

namespace Tags {

class ShaderProgramTag;

} // namespace Tags

using ShaderProgramGL4 = pomdog::detail::Tagged<GLuint, Tags::ShaderProgramTag>;

} // namespace pomdog::gpu::detail::gl4
