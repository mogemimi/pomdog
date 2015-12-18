// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_OPENGLPREREQUISITES_70C95DBA_HPP
#define POMDOG_OPENGLPREREQUISITES_70C95DBA_HPP

#include "Pomdog/Basic/Platform.hpp"

#if defined(POMDOG_PLATFORM_WIN32)
#include <GL/glew.h>
#include <GL/wglew.h>
#elif defined(POMDOG_PLATFORM_MACOSX)
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#elif defined(POMDOG_PLATFORM_APPLE_IOS)
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#elif defined(POMDOG_PLATFORM_LINUX)
#include <GL/glew.h>
#endif

#endif // POMDOG_OPENGLPREREQUISITES_70C95DBA_HPP
