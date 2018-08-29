// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "KeyboardWin32.hpp"
#include "Pomdog/Input/KeyState.hpp"
#include "Pomdog/Input/Keys.hpp"
#include <optional>

namespace Pomdog {
namespace Detail {
namespace Win32 {
namespace {

std::optional<Keys> ToKeys(USHORT keyCode) noexcept
{
    switch (keyCode) {
    // Function keys
    case VK_F1: return Keys::F1;
    case VK_F2: return Keys::F2;
    case VK_F3: return Keys::F3;
    case VK_F4: return Keys::F4;
    case VK_F5: return Keys::F5;
    case VK_F6: return Keys::F6;
    case VK_F7: return Keys::F7;
    case VK_F8: return Keys::F8;
    case VK_F9: return Keys::F9;
    case VK_F10: return Keys::F10;
    case VK_F11: return Keys::F11;
    case VK_F12: return Keys::F12;
    case VK_F13: return Keys::F13;
    case VK_F14: return Keys::F14;
    case VK_F15: return Keys::F15;

    // Alphanumerical keys
    case 0x41: return Keys::A;
    case 0x42: return Keys::B;
    case 0x43: return Keys::C;
    case 0x44: return Keys::D;
    case 0x45: return Keys::E;
    case 0x46: return Keys::F;
    case 0x47: return Keys::G;
    case 0x48: return Keys::H;
    case 0x49: return Keys::I;
    case 0x4A: return Keys::J;
    case 0x4B: return Keys::K;
    case 0x4C: return Keys::L;
    case 0x4D: return Keys::M;
    case 0x4E: return Keys::N;
    case 0x4F: return Keys::O;
    case 0x50: return Keys::P;
    case 0x51: return Keys::Q;
    case 0x52: return Keys::R;
    case 0x53: return Keys::S;
    case 0x54: return Keys::T;
    case 0x55: return Keys::U;
    case 0x56: return Keys::V;
    case 0x57: return Keys::W;
    case 0x58: return Keys::X;
    case 0x59: return Keys::Y;
    case 0x5A: return Keys::Z;
    case 0x30: return Keys::Alpha0;
    case 0x31: return Keys::Alpha1;
    case 0x32: return Keys::Alpha2;
    case 0x33: return Keys::Alpha3;
    case 0x34: return Keys::Alpha4;
    case 0x35: return Keys::Alpha5;
    case 0x36: return Keys::Alpha6;
    case 0x37: return Keys::Alpha7;
    case 0x38: return Keys::Alpha8;
    case 0x39: return Keys::Alpha9;

    // Numeric keypad keys
    case VK_NUMPAD0: return Keys::Keypad0;
    case VK_NUMPAD1: return Keys::Keypad1;
    case VK_NUMPAD2: return Keys::Keypad2;
    case VK_NUMPAD3: return Keys::Keypad3;
    case VK_NUMPAD4: return Keys::Keypad4;
    case VK_NUMPAD5: return Keys::Keypad5;
    case VK_NUMPAD6: return Keys::Keypad6;
    case VK_NUMPAD7: return Keys::Keypad7;
    case VK_NUMPAD8: return Keys::Keypad8;
    case VK_NUMPAD9: return Keys::Keypad9;
    case VK_DECIMAL: return Keys::KeypadDecimal;
    case VK_ADD: return Keys::KeypadAdd;
    case VK_DIVIDE: return Keys::KeypadDivide;
    case VK_MULTIPLY: return Keys::KeypadMultiply;
    case VK_SUBTRACT: return Keys::KeypadSubtract;

    // Keys on Arrow keypad
    case VK_UP: return Keys::UpArrow;
    case VK_DOWN: return Keys::DownArrow;
    case VK_LEFT: return Keys::LeftArrow;
    case VK_RIGHT: return Keys::RightArrow;
    case VK_INSERT: return Keys::Insert;
    case VK_HOME: return Keys::Home;
    case VK_END: return Keys::End;
    case VK_NEXT: return Keys::PageDown;
    case VK_PRIOR: return Keys::PageUp;

    // Special keys
    case VK_BACK: return Keys::BackSpace;
    case VK_DELETE: return Keys::Delete;
    case VK_TAB: return Keys::Tab;
    case VK_RETURN: return Keys::Enter;
    case VK_PAUSE: return Keys::Pause;
    case VK_ESCAPE: return Keys::Escape;
    case VK_SPACE: return Keys::Space;
    case VK_SNAPSHOT: return Keys::PrintScreen;
    case VK_NUMLOCK: return Keys::NumLock;
    case VK_SCROLL: return Keys::ScrollLock;
    case VK_CAPITAL: return Keys::CapsLock;
    case VK_MENU: return Keys::LeftAlt;
    case VK_CONTROL: return Keys::LeftControl;
    case VK_LCONTROL: return Keys::LeftControl;
    case VK_RCONTROL: return Keys::RightControl;
    case VK_SHIFT: return Keys::LeftShift;
    case VK_LSHIFT: return Keys::LeftShift;
    case VK_RSHIFT: return Keys::RightControl;
    case VK_HELP: return Keys::Help;
    case VK_CLEAR : return Keys::Clear;
    case VK_PRINT: return Keys::Print;

    // OEM keys
    case VK_OEM_COMMA: return Keys::Comma;
    case VK_OEM_PLUS: return Keys::Plus;
    case VK_OEM_PERIOD: return Keys::Period;
    case VK_OEM_MINUS: return Keys::Minus;
    case VK_OEM_102: return Keys::Backslash;
    case VK_OEM_1: return Keys::Semicolon;
    case VK_OEM_2: return Keys::Question;
    case VK_OEM_3: return Keys::AccentGrave;
    case VK_OEM_7: return Keys::Quote;
    case VK_OEM_4: return Keys::OpenBracket;
    case VK_OEM_6: return Keys::CloseBracket;

    //case: return Keys::Equals;
    //case: return Keys::;
    //case : return Keys::Tilde;
    //case : return Keys::DoubleQuote;

    // Windows, Apple keys
    case VK_LWIN: return Keys::LeftWindows;
    case VK_RWIN: return Keys::RightWindows;

    // IME keys
    case VK_CONVERT: return Keys::IMEConvert;
    case VK_NONCONVERT: return Keys::IMENoConvert;
    case VK_KANA: return Keys::KanaMode;
    case VK_KANJI: return Keys::KanjiMode;
    default:
        break;
    }

    return std::nullopt;
}

} // unnamed namespace

void KeyboardWin32::HandleMessage(const RAWKEYBOARD& keyboard)
{
    const auto key = ToKeys(keyboard.VKey);
    const auto keyState = (keyboard.Flags & RI_KEY_BREAK ? KeyState::Up : KeyState::Down);

    if (!key) {
        return;
    }

    bool isKeyDown = keyboardState.IsKeyDown(*key);
    keyboardState.SetKey(*key, keyState);

    switch (keyState) {
    case KeyState::Down:
        if (!isKeyDown) {
            Keyboard::KeyDown(*key);
        }
        break;
    case KeyState::Up:
        if (isKeyDown) {
            Keyboard::KeyUp(*key);
        }
        break;
    }
}

KeyboardState KeyboardWin32::GetState() const
{
    return keyboardState;
}

} // namespace Win32
} // namespace Detail
} // namespace Pomdog
