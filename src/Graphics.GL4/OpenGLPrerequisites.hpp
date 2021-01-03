// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Platform.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
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
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END
