//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_OPENGLPREREQUISITES_70C95DBA_5F82_41A6_A732_5FB0470F0A3D_HPP
#define POMDOG_OPENGLPREREQUISITES_70C95DBA_5F82_41A6_A732_5FB0470F0A3D_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Basic/Platform.hpp"

#if defined(POMDOG_PLATFORM_WIN32)
#	include <GL/glew.h>
#	include <GL/wglew.h>
#elif defined(POMDOG_PLATFORM_MACOSX)
#	include <OpenGL/gl3.h>
#	include <OpenGL/gl3ext.h>
#elif defined(POMDOG_PLATFORM_LINUX)
#	include <GL/glew.h>
#endif

#endif // !defined(POMDOG_OPENGLPREREQUISITES_70C95DBA_5F82_41A6_A732_5FB0470F0A3D_HPP)
