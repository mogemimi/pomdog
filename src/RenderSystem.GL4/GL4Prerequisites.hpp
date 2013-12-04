//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_GL4_PREREQUISITES_HPP
#define POMDOG_SRC_GL4_PREREQUISITES_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Config/Platform.hpp>

#if defined(POMDOG_PLATFORM_WIN32)
#	include <GL/glew.h>
#	include <GL/wglew.h>
#	//include <GL/GLU.h>
#elif defined(POMDOG_PLATFORM_LINUX)
#	include <GL/glew.h>
#	//include <GL/GLU.h>
#elif defined(POMDOG_PLATFORM_MACOSX)
#   include <OpenGL/gl.h>
#   //include <OpenGL/glu.h>
#endif

#endif // !defined(POMDOG_SRC_GL4_PREREQUISITES_HPP)
