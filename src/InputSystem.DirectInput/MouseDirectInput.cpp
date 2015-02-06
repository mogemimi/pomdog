//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "MouseDirectInput.hpp"
#include "../Application/SubsystemScheduler.hpp"
#include "Pomdog/Utility/Assert.hpp"
//#include "Pomdog/Utility/Exception.hpp"
#if defined(DEBUG) && !defined(NDEBUG)
#include "Pomdog/Logging/Log.hpp"
#endif
#include <array>

namespace Pomdog {
namespace Details {
namespace InputSystem {
namespace DirectInput {
namespace {

static ButtonState ToButtonState(DWORD source)
{
	return (0x80 == (source & 0x80)
		? ButtonState::Pressed
		: ButtonState::Released);
}
//-----------------------------------------------------------------------
static void TranslateToMouseState(DIDEVICEOBJECTDATA const& source, MouseState & mouseState)
{
	switch (source.dwOfs) {
	case DIMOFS_Z: {
		mouseState.ScrollWheel = source.dwData;
		break;
	}
	case DIMOFS_BUTTON0: {
		mouseState.LeftButton = ToButtonState(source.dwData);
		break;
	}
	case DIMOFS_BUTTON1: {
		mouseState.RightButton = ToButtonState(source.dwData);
		break;
	}
	case DIMOFS_BUTTON2: {
		mouseState.MiddleButton = ToButtonState(source.dwData);
		break;
	}
	case DIMOFS_BUTTON3: {
		mouseState.XButton1 = ToButtonState(source.dwData);
		break;
	}
	case DIMOFS_BUTTON4: {
		mouseState.XButton2 = ToButtonState(source.dwData);
		break;
	}
	default:
		break;
	}
}

}// unnamed namespace
//-----------------------------------------------------------------------
MouseDirectInput::MouseDirectInput(IDirectInput8* directInput,
	HWND windowHandleIn, SubsystemScheduler & scheduler)
	: windowHandle(windowHandleIn)
{
	POMDOG_ASSERT(directInput);
	POMDOG_ASSERT(windowHandle);

	HRESULT hr = directInput->CreateDevice(GUID_SysMouse, &inputDevice, nullptr);
	if (FAILED(hr)) {
		// error: FUS RO DAH!
		///@todo Not implemented
	}

	POMDOG_ASSERT(inputDevice);

	hr = inputDevice->SetDataFormat(&c_dfDIMouse2);
	if (FAILED(hr)) {
		inputDevice.Reset();
		// error: FUS RO DAH!
		///@todo Not implemented
	}

	hr = inputDevice->SetCooperativeLevel(windowHandle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) {
		inputDevice.Reset();
		// error: FUS RO DAH!
		///@todo Not implemented
	}

	DIPROPDWORD directInputProps;
	directInputProps.diph.dwSize = sizeof(DIPROPDWORD);
	directInputProps.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	directInputProps.diph.dwObj = 0;
	directInputProps.diph.dwHow = DIPH_DEVICE;
	directInputProps.dwData = MaxKeyBufferSize;

	hr = inputDevice->SetProperty(DIPROP_BUFFERSIZE, &directInputProps.diph);
	if (FAILED(hr)) {
		inputDevice.Reset();
		// error: FUS RO DAH!
		///@todo Not implemented
	}

	directInputProps.dwData = DIPROPAXISMODE_ABS;
	hr = inputDevice->SetProperty(DIPROP_AXISMODE, &directInputProps.diph);
	if (FAILED(hr)) {
		inputDevice.Reset();
		// error: FUS RO DAH!
		///@todo Not implemented
	}

	connection = scheduler.OnUpdate.Connect([this] {
		this->Update();
	});
}
//-----------------------------------------------------------------------
MouseDirectInput::~MouseDirectInput()
{
	if (inputDevice) {
		inputDevice->Unacquire();
		inputDevice.Reset();
	}
}
//-----------------------------------------------------------------------
MouseState MouseDirectInput::GetState() const
{
	return mouseState;
}
//-----------------------------------------------------------------------
void MouseDirectInput::Update()
{
	POMDOG_ASSERT(inputDevice);

	HRESULT hr = DI_OK;

	hr = inputDevice->Acquire();
	if (FAILED(hr)) {
		return;
	}

#if defined(DEBUG) && !defined(NDEBUG)
	{
		DWORD objectCount = INFINITE;
		hr = inputDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), nullptr, &objectCount, DIGDD_PEEK);
		if (hr == DI_BUFFEROVERFLOW)
		{
			if (objectCount >= MaxKeyBufferSize) {
				Log::Warning("Pomdog", "Buffer-overflow occurred!"
					"Not all data was successfully captured. Number of elements in buffer: "
					+ std::to_string(objectCount));
			}
		}
	}
#endif

	std::array<DIDEVICEOBJECTDATA, MaxKeyBufferSize> deviceObjects;

	DWORD objectCount = deviceObjects.size();

	hr = inputDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),
		deviceObjects.data(), &objectCount, 0);

	if (FAILED(hr))
	{
		POMDOG_ASSERT(hr != DIERR_INVALIDPARAM);
		POMDOG_ASSERT(hr != DIERR_NOTBUFFERED);
		return;
	}

	POINT cursorPos;
	GetCursorPos(&cursorPos);

	if (windowHandle != nullptr) {
		ScreenToClient(windowHandle, &cursorPos);
	}

	mouseState.Position.X = cursorPos.x;
	mouseState.Position.Y = cursorPos.y;

	for (DWORD index = 0; index < objectCount; ++index) {
		TranslateToMouseState(deviceObjects[index], mouseState);
	}
}
//-----------------------------------------------------------------------
}// namespace DirectInput
}// namespace InputSystem
}// namespace Details
}// namespace Pomdog
