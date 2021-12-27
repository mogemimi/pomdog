// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#if defined(_XBOX_ONE) && defined(_TITLE)
#include <d3d11_x.h>
#elif (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/) || defined(_WIN7_PLATFORM_UPDATE)
#include <d3d11_3.h>
#include <d3d11shader.h>
#else
#include <d3d11.h>
#include <d3d11shader.h>
#endif
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END
