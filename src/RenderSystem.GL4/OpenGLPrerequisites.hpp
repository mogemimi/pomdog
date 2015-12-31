// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

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
