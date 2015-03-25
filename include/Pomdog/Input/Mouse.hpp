// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_MOUSE_8BFC478A_C7F2_4D4E_A4FE_C756500D5E33_HPP
#define POMDOG_MOUSE_8BFC478A_C7F2_4D4E_A4FE_C756500D5E33_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Basic/Export.hpp"

namespace Pomdog {

class MouseState;

class POMDOG_EXPORT Mouse {
public:
	Mouse() = default;
	Mouse(Mouse const&) = delete;
	Mouse & operator=(Mouse const&) = delete;

	virtual ~Mouse() = default;

	virtual MouseState GetState() const = 0;
};

}// namespace Pomdog

#endif // !defined(POMDOG_MOUSE_8BFC478A_C7F2_4D4E_A4FE_C756500D5E33_HPP)
