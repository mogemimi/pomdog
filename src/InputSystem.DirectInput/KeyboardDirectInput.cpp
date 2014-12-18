//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "KeyboardDirectInput.hpp"
#include "../Application/SubsystemScheduler.hpp"
#include "Pomdog/Input/Keys.hpp"
#include "Pomdog/Input/KeyState.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Optional.hpp"
//#include "Pomdog/Utility/Exception.hpp"
#if defined(DEBUG) && !defined(NDEBUG)
#include "Pomdog/Logging/Log.hpp"
#endif
#include <array>
#include <utility>

namespace Pomdog {
namespace Details {
namespace InputSystem {
namespace DirectInput {
namespace {

static Optional<Keys> ToKeys(::UINT directInputKeycode)
{
	switch (directInputKeycode) {
	// Function keys
	case DIK_F1: return Keys::F1;
	case DIK_F2: return Keys::F2;
	case DIK_F3: return Keys::F3;
	case DIK_F4: return Keys::F4;
	case DIK_F5: return Keys::F5;
	case DIK_F6: return Keys::F6;
	case DIK_F7: return Keys::F7;
	case DIK_F8: return Keys::F8;
	case DIK_F9: return Keys::F9;
	case DIK_F10: return Keys::F10;
	case DIK_F11: return Keys::F11;
	case DIK_F12: return Keys::F12;
	case DIK_F13: return Keys::F13;
	case DIK_F14: return Keys::F14;
	case DIK_F15: return Keys::F15;

	// Alphanumerical keys
	case DIK_A: return Keys::A;
	case DIK_B: return Keys::B;
	case DIK_C: return Keys::C;
	case DIK_D: return Keys::D;
	case DIK_E: return Keys::E;
	case DIK_F: return Keys::F;
	case DIK_G: return Keys::G;
	case DIK_H: return Keys::H;
	case DIK_I: return Keys::I;
	case DIK_J: return Keys::J;
	case DIK_K: return Keys::K;
	case DIK_L: return Keys::L;
	case DIK_M: return Keys::M;
	case DIK_N: return Keys::N;
	case DIK_O: return Keys::O;
	case DIK_P: return Keys::P;
	case DIK_Q: return Keys::Q;
	case DIK_R: return Keys::R;
	case DIK_S: return Keys::S;
	case DIK_T: return Keys::T;
	case DIK_U: return Keys::U;
	case DIK_V: return Keys::V;
	case DIK_W: return Keys::W;
	case DIK_X: return Keys::X;
	case DIK_Y: return Keys::Y;
	case DIK_Z: return Keys::Z;
	case DIK_0: return Keys::Alpha0;
	case DIK_1: return Keys::Alpha1;
	case DIK_2: return Keys::Alpha2;
	case DIK_3: return Keys::Alpha3;
	case DIK_4: return Keys::Alpha4;
	case DIK_5: return Keys::Alpha5;
	case DIK_6: return Keys::Alpha6;
	case DIK_7: return Keys::Alpha7;
	case DIK_8: return Keys::Alpha8;
	case DIK_9: return Keys::Alpha9;

	// Numeric keypad keys
	case DIK_NUMPAD0: return Keys::Keypad0;
	case DIK_NUMPAD1: return Keys::Keypad1;
	case DIK_NUMPAD2: return Keys::Keypad2;
	case DIK_NUMPAD3: return Keys::Keypad3;
	case DIK_NUMPAD4: return Keys::Keypad4;
	case DIK_NUMPAD5: return Keys::Keypad5;
	case DIK_NUMPAD6: return Keys::Keypad6;
	case DIK_NUMPAD7: return Keys::Keypad7;
	case DIK_NUMPAD8: return Keys::Keypad8;
	case DIK_NUMPAD9: return Keys::Keypad9;
	case DIK_DECIMAL: return Keys::KeypadDecimal;
	case DIK_ADD: return Keys::KeypadAdd;
	case DIK_DIVIDE: return Keys::KeypadDivide;
	case DIK_MULTIPLY: return Keys::KeypadMultiply;
	case DIK_SUBTRACT: return Keys::KeypadSubtract;

	// Keys on Arrow keypad
	case DIK_UPARROW: return Keys::UpArrow;
	case DIK_DOWNARROW: return Keys::DownArrow;
	case DIK_LEFTARROW: return Keys::LeftArrow;
	case DIK_RIGHTARROW: return Keys::RightArrow;
	case DIK_INSERT: return Keys::Insert;
	case DIK_HOME: return Keys::Home;
	case DIK_END: return Keys::End;
	case DIK_PGDN: return Keys::PageDown;
	case DIK_PGUP: return Keys::PageUp;

	// Special keys
	case DIK_BACKSPACE: return Keys::BackSpace;
	case DIK_DELETE: return Keys::Delete;
	case DIK_TAB: return Keys::Tab;
	case DIK_RETURN: return Keys::Enter;
	case DIK_PAUSE: return Keys::Pause;
	case DIK_ESCAPE: return Keys::Escape;
	case DIK_SPACE: return Keys::Space;
	case DIK_SYSRQ: return Keys::PrintScreen;
	case DIK_NUMLOCK: return Keys::NumLock;
	case DIK_SCROLL: return Keys::ScrollLock;
	case DIK_CAPSLOCK: return Keys::CapsLock;
	case DIK_LALT: return Keys::LeftAlt;
	case DIK_RALT: return Keys::RightAlt;
	case DIK_LCONTROL: return Keys::LeftControl;
	case DIK_RCONTROL: return Keys::RightControl;
	case DIK_LSHIFT: return Keys::LeftShift;
	case DIK_RSHIFT: return Keys::RightControl;

	//case : return Keys::Clear;
	//case : return Keys::Help;
	//case : return Keys::Print;

	// OEM keys
	case DIK_COMMA: return Keys::Comma;
	case DIK_EQUALS: return Keys::Equals;
	case DIK_SEMICOLON: return Keys::Semicolon;
	case DIK_PERIOD: return Keys::Period;
	case DIK_MINUS: return Keys::Minus;
	case DIK_APOSTROPHE: return Keys::Quote;
	case DIK_BACKSLASH: return Keys::Backslash;
	
	//case : return Keys::Plus;
	//case : return Keys::Question;
	//case : return Keys::DoubleQuote;
	//case : return Keys::Tilde;

	case DIK_LBRACKET: return Keys::OpenBracket;
	case DIK_RBRACKET: return Keys::CloseBracket;
	case DIK_GRAVE: return Keys::AccentGrave;

	// Windows, Apple keys
	case DIK_LWIN: return Keys::LeftWindows;
	case DIK_RWIN: return Keys::RightWindows;

	// IME keys
	case DIK_CONVERT: return Keys::IMEConvert;
	case DIK_NOCONVERT: return Keys::IMENoConvert;
	case DIK_KANA: return Keys::KanaMode;
	case DIK_KANJI: return Keys::KanjiMode;
	default:
		break;
	}

	return OptionalType::NullOptional;
}

}// unnamed namespace
//-----------------------------------------------------------------------
KeyboardDirectInput::KeyboardDirectInput(IDirectInput8* directInput,
	HWND windowHandle, SubsystemScheduler & scheduler)
{
	POMDOG_ASSERT(directInput);
	POMDOG_ASSERT(windowHandle);

	HRESULT hr = directInput->CreateDevice(GUID_SysKeyboard, &inputDevice, nullptr);
	if (FAILED(hr)) {
		// error: FUS RO DAH!
		///@todo Not implemented
	}

	POMDOG_ASSERT(inputDevice);

	hr = inputDevice->SetDataFormat(&c_dfDIKeyboard);
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

	connection = scheduler.OnUpdate.Connect([this] {
		this->Update();
	});
}
//-----------------------------------------------------------------------
KeyboardDirectInput::~KeyboardDirectInput()
{
	if (inputDevice) {
		inputDevice->Unacquire();
		inputDevice.Reset();
	}
}
//-----------------------------------------------------------------------
KeyboardState KeyboardDirectInput::GetState() const
{
	return keyboardState;
}
//-----------------------------------------------------------------------
void KeyboardDirectInput::Update()
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

	for (DWORD index = 0; index < objectCount; ++index)
	{
		auto key = ToKeys(deviceObjects[index].dwOfs);
		if (!key) {
			continue;
		}

		auto keyState = (0x80 == (deviceObjects[index].dwData & 0x80)) ?
			KeyState::Down : KeyState::Up;

		keyboardState.SetKey(*key, keyState);
	}

	POMDOG_ASSERT(keyboardState[Keys::None] == KeyState::Up);
}
//-----------------------------------------------------------------------
}// namespace DirectInput
}// namespace InputSystem
}// namespace Details
}// namespace Pomdog
