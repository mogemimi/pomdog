// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"

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
#if defined(__clang__)
#pragma clang diagnostic push
#if (__clang_major__ >= 13) && !defined(__APPLE_CC__)
#pragma clang diagnostic ignored "-Wreserved-identifier"
#endif
#endif
#include <GL/glew.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#endif
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END
