//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SRC_GL4_TYPESAFEGL4_B23282F8_F9C8_416A_81F6_FC0BC2E63525_HPP
#define POMDOG_SRC_GL4_TYPESAFEGL4_B23282F8_F9C8_416A_81F6_FC0BC2E63525_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "OpenGLPrerequisites.hpp"
#include <Pomdog/Utility/detail/Tagged.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

namespace Tags {

class ShaderProgramTag;

}// namespace Tags

using ShaderProgramGL4 = Tagged<GLuint, Tags::ShaderProgramTag>;

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_GL4_TYPESAFEGL4_B23282F8_F9C8_416A_81F6_FC0BC2E63525_HPP)
