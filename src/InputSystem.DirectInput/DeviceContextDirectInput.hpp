// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_DEVICECONTEXTDIRECTINPUT_A3771E91_HPP
#define POMDOG_DEVICECONTEXTDIRECTINPUT_A3771E91_HPP

#include "PrerequisitesDirectInput.hpp"
#include "../Utility/Noncopyable.hpp"
#include <wrl/client.h>
#include <memory>

namespace Pomdog {
namespace Detail {
namespace InputSystem {
namespace DirectInput {

class DeviceContextDirectInput final : Noncopyable {
public:
    DeviceContextDirectInput(HINSTANCE hInstance, HWND windowHandle);

    ~DeviceContextDirectInput();

    HWND WindowHandle() const;

    IDirectInput8* GetDirectInput() const;

private:
    HWND windowHandle;
    Microsoft::WRL::ComPtr<IDirectInput8> directInput;
};

} // namespace DirectInput
} // namespace InputSystem
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_DEVICECONTEXTDIRECTINPUT_A3771E91_HPP
