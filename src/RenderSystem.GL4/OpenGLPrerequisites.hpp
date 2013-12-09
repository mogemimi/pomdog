//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_GL4_OPENGL_PREREQUISITES_8F68D600_60F5_11E3_BF2F_A8206655A22B_HPP
#define POMDOG_SRC_GL4_OPENGL_PREREQUISITES_8F68D600_60F5_11E3_BF2F_A8206655A22B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Config/Platform.hpp>

#if defined(POMDOG_PLATFORM_WIN32)
#	include <GL/glew.h>
#	include <GL/wglew.h>
#elif defined(POMDOG_PLATFORM_MACOSX)
#	include <OpenGL/gl3.h>
#   include <OpenGL/gl3ext.h>
#elif defined(POMDOG_PLATFORM_LINUX)
#	include <GL/glew.h>
#endif

#endif // !defined(POMDOG_SRC_GL4_OPENGL_PREREQUISITES_8F68D600_60F5_11E3_BF2F_A8206655A22B_HPP)
