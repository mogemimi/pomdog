// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/gl4/opengl_prerequisites.hpp"
#include "pomdog/utility/tagged.hpp"

namespace Pomdog::Detail::GL4 {

namespace Tags {

class ShaderProgramTag;

} // namespace Tags

using ShaderProgramGL4 = Tagged<GLuint, Tags::ShaderProgramTag>;

} // namespace Pomdog::Detail::GL4
