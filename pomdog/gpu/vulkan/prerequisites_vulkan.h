// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"

#ifdef POMDOG_PLATFORM_WIN32
// NOTE: You need to include Windows.h before including <vulkan/vulkan.h>.
//	     Also, you need to define NOMINMAX and WIN32_LEAN_AND_MEAN.
#include "pomdog/platform/win32/prerequisites_win32.h"
#endif

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vulkan/vulkan.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END
