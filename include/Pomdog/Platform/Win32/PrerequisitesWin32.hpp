//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_PREREQUISITESWIN32_ABB52E43_65B2_45DC_8F30_310E7A8B50A3_HPP
#define POMDOG_PREREQUISITESWIN32_ABB52E43_65B2_45DC_8F30_310E7A8B50A3_HPP

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

#include <tchar.h>
#include <Windows.h>

#endif // !defined(POMDOG_PREREQUISITESWIN32_ABB52E43_65B2_45DC_8F30_310E7A8B50A3_HPP)
