// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/gl4/opengl_prerequisites.h"
#include "pomdog/utility/tagged.h"

namespace pomdog::detail::gl4 {

namespace Tags {

class ShaderProgramTag;

} // namespace Tags

using ShaderProgramGL4 = Tagged<GLuint, Tags::ShaderProgramTag>;

} // namespace pomdog::detail::gl4
