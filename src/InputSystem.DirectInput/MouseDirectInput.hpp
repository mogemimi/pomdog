//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MOUSEDIRECTINPUT_02D734AC_5354_4EBE_988D_B35AE3CCEE38_HPP
#define POMDOG_MOUSEDIRECTINPUT_02D734AC_5354_4EBE_988D_B35AE3CCEE38_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "PrerequisitesDirectInput.hpp"
#include "Pomdog/Input/Mouse.hpp"
#include "Pomdog/Input/MouseState.hpp"
#include "Pomdog/Event/ScopedConnection.hpp"
#include <wrl/client.h>

namespace Pomdog {
namespace Details {

class SubsystemScheduler;

namespace InputSystem {
namespace DirectInput {

class MouseDirectInput final: public Mouse {
public:
	MouseDirectInput(IDirectInput8* directInput, HWND windowHandle,
		SubsystemScheduler & scheduler);

	~MouseDirectInput();

	MouseState GetState() const override;

private:
	void Update();

private:
	ScopedConnection connection;
	MouseState mouseState;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> inputDevice;
	HWND windowHandle;
	static constexpr std::size_t MaxKeyBufferSize = 16;
};

}// namespace DirectInput
}// namespace InputSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_MOUSEDIRECTINPUT_02D734AC_5354_4EBE_988D_B35AE3CCEE38_HPP)
