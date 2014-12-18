//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SRC_WIN32_WIN32PREREQUISITES_E2FCD355_BD15_456C_BB43_2E6352246099_HPP
#define POMDOG_SRC_WIN32_WIN32PREREQUISITES_E2FCD355_BD15_456C_BB43_2E6352246099_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#ifndef STRICT
#	define STRICT
#endif
#ifndef	WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN
#endif
#if !defined(NOMINMAX) && defined(_MSC_VER)
#	define NOMINMAX
#endif

#include <Windows.h>

#endif // !defined(POMDOG_SRC_WIN32_WIN32PREREQUISITES_E2FCD355_BD15_456C_BB43_2E6352246099_HPP)
