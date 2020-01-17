// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include "../Utility/Tagged.hpp"

namespace Pomdog::Detail::GL4 {

namespace Tags {

class ShaderProgramTag;

} // namespace Tags

using ShaderProgramGL4 = Tagged<GLuint, Tags::ShaderProgramTag>;

} // namespace Pomdog::Detail::GL4
