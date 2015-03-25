// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TYPESAFEGL4_B23282F8_F9C8_416A_81F6_FC0BC2E63525_HPP
#define POMDOG_TYPESAFEGL4_B23282F8_F9C8_416A_81F6_FC0BC2E63525_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "OpenGLPrerequisites.hpp"
#include "Pomdog/Utility/detail/Tagged.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace GL4 {

namespace Tags {

class ShaderProgramTag;

}// namespace Tags

using ShaderProgramGL4 = Tagged<GLuint, Tags::ShaderProgramTag>;

}// namespace GL4
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_TYPESAFEGL4_B23282F8_F9C8_416A_81F6_FC0BC2E63525_HPP)
