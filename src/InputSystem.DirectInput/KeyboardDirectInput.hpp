//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_KEYBOARDDIRECTINPUT_C564C1C5_4C17_432D_8E0D_7673012F1D0D_HPP
#define POMDOG_KEYBOARDDIRECTINPUT_C564C1C5_4C17_432D_8E0D_7673012F1D0D_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "PrerequisitesDirectInput.hpp"
#include "Pomdog/Input/Keyboard.hpp"
#include "Pomdog/Input/KeyboardState.hpp"
#include "Pomdog/Event/ScopedConnection.hpp"
#include <wrl/client.h>

namespace Pomdog {
namespace Details {

class SubsystemScheduler;

namespace InputSystem {
namespace DirectInput {

class KeyboardDirectInput final: public Keyboard {
public:
	KeyboardDirectInput(IDirectInput8* directInput, HWND windowHandle,
		SubsystemScheduler & scheduler);

	~KeyboardDirectInput();

	KeyboardState GetState() const override;

private:
	void Update();

private:
	ScopedConnection connection;
	KeyboardState keyboardState;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> inputDevice;
	static constexpr std::size_t MaxKeyBufferSize = 8;
};

}// namespace DirectInput
}// namespace InputSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_KEYBOARDDIRECTINPUT_C564C1C5_4C17_432D_8E0D_7673012F1D0D_HPP)
