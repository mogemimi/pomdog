// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_KEYBOARDDIRECTINPUT_C564C1C5_HPP
#define POMDOG_KEYBOARDDIRECTINPUT_C564C1C5_HPP

#include "PrerequisitesDirectInput.hpp"
#include "Pomdog/Input/Keyboard.hpp"
#include "Pomdog/Input/KeyboardState.hpp"
#include "Pomdog/Event/ScopedConnection.hpp"
#include <wrl/client.h>

namespace Pomdog {
namespace Detail {

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
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_KEYBOARDDIRECTINPUT_C564C1C5_HPP
