// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include "Pomdog/Utility/detail/Tagged.hpp"

namespace Pomdog {
namespace Detail {
namespace GL4 {

namespace Tags {

class ShaderProgramTag;

} // namespace Tags

using ShaderProgramGL4 = Tagged<GLuint, Tags::ShaderProgramTag>;

} // namespace GL4
} // namespace Detail
} // namespace Pomdog
