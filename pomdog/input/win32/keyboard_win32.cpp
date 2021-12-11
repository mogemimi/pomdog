// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/win32/keyboard_win32.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/input/key_state.hpp"
#include "pomdog/input/keys.hpp"
#include "pomdog/signals/event_queue.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::win32 {
namespace {

Keys TranslateKey(std::uint16_t keyCode) noexcept
{
    constexpr std::array<Keys, 228> keyTable = {
        Keys::Unknown,         // 0x01: VK_LBUTTON
        Keys::Unknown,         // 0x02: VK_RBUTTON
        Keys::Cancel,          // 0x03: VK_CANCEL
        Keys::Unknown,         // 0x04: VK_MBUTTON
        Keys::Unknown,         // 0x05: VK_XBUTTON1
        Keys::Unknown,         // 0x06: VK_XBUTTON2
        Keys::Unknown,         // 0x07:
        Keys::BackSpace,       // 0x08: VK_BACK
        Keys::Tab,             // 0x09: VK_TAB
        Keys::Unknown,         // 0x0A:
        Keys::Unknown,         // 0x0B:
        Keys::Clear,           // 0x0C: VK_CLEAR
        Keys::Enter,           // 0x0D: VK_RETURN
        Keys::Unknown,         // 0x0E:
        Keys::Unknown,         // 0x0F:
        Keys::LeftShift,       // 0x10: VK_SHIFT
        Keys::LeftControl,     // 0x11: VK_CONTROL
        Keys::LeftAlt,         // 0x12: VK_MENU
        Keys::Pause,           // 0x13: VK_PAUSE
        Keys::CapsLock,        // 0x14: VK_CAPITAL
        Keys::KanaMode,        // 0x15: VK_KANA, VK_HANGUL
        Keys::Unknown,         // 0x16:
        Keys::JunjaMode,       // 0x17: VK_JUNJA
        Keys::FinalMode,       // 0x18: VK_FINAL
        Keys::KanjiMode,       // 0x19: VK_KANJI, VK_HANJA
        Keys::Unknown,         // 0x1A:
        Keys::Escape,          // 0x1B: VK_ESCAPE
        Keys::IMEConvert,      // 0x1C: VK_CONVERT
        Keys::IMENonConvert,   // 0x1D: VK_NONCONVERT
        Keys::Accept,          // 0x1E: VK_ACCEPT
        Keys::ModeChange,      // 0x1F: VK_MODECHANGE
        Keys::Space,           // 0x20: VK_SPACE
        Keys::PageUp,          // 0x21: VK_PRIOR
        Keys::PageDown,        // 0x22: VK_NEXT
        Keys::End,             // 0x23: VK_END
        Keys::Home,            // 0x24: VK_HOME
        Keys::LeftArrow,       // 0x25: VK_LEFT
        Keys::UpArrow,         // 0x26: VK_UP
        Keys::RightArrow,      // 0x27: VK_RIGHT
        Keys::DownArrow,       // 0x28: VK_DOWN
        Keys::Select,          // 0x29: VK_SELECT
        Keys::Print,           // 0x2A: VK_PRINT
        Keys::Execute,         // 0x2B: VK_EXECUTE
        Keys::PrintScreen,     // 0x2C: VK_SNAPSHOT
        Keys::Insert,          // 0x2D: VK_INSERT
        Keys::Delete,          // 0x2E: VK_DELETE
        Keys::Help,            // 0x2F: VK_HELP
        Keys::Alpha0,          // 0x30:
        Keys::Alpha1,          // 0x31:
        Keys::Alpha2,          // 0x32:
        Keys::Alpha3,          // 0x33:
        Keys::Alpha4,          // 0x34:
        Keys::Alpha5,          // 0x35:
        Keys::Alpha6,          // 0x36:
        Keys::Alpha7,          // 0x37:
        Keys::Alpha8,          // 0x38:
        Keys::Alpha9,          // 0x39:
        Keys::Unknown,         // 0x3A:
        Keys::Unknown,         // 0x3B:
        Keys::Unknown,         // 0x3C:
        Keys::Unknown,         // 0x3D:
        Keys::Unknown,         // 0x3E:
        Keys::Unknown,         // 0x3F:
        Keys::Unknown,         // 0x40:
        Keys::A,               // 0x41:
        Keys::B,               // 0x42:
        Keys::C,               // 0x43:
        Keys::D,               // 0x44:
        Keys::E,               // 0x45:
        Keys::F,               // 0x46:
        Keys::G,               // 0x47:
        Keys::H,               // 0x48:
        Keys::I,               // 0x49:
        Keys::J,               // 0x4A:
        Keys::K,               // 0x4B:
        Keys::L,               // 0x4C:
        Keys::M,               // 0x4D:
        Keys::N,               // 0x4E:
        Keys::O,               // 0x4F:
        Keys::P,               // 0x50:
        Keys::Q,               // 0x51:
        Keys::R,               // 0x52:
        Keys::S,               // 0x53:
        Keys::T,               // 0x54:
        Keys::U,               // 0x55:
        Keys::V,               // 0x56:
        Keys::W,               // 0x57:
        Keys::X,               // 0x58:
        Keys::Y,               // 0x59:
        Keys::Z,               // 0x5A:
        Keys::LeftSuper,       // 0x5B: VK_LWIN
        Keys::RightSuper,      // 0x5C: VK_RWIN
        Keys::Applications,    // 0x5D: VK_APPS
        Keys::Unknown,         // 0x5E:
        Keys::Sleep,           // 0x5F: VK_SLEEP
        Keys::Keypad0,         // 0x60: VK_NUMPAD0
        Keys::Keypad1,         // 0x61: VK_NUMPAD1
        Keys::Keypad2,         // 0x62: VK_NUMPAD2
        Keys::Keypad3,         // 0x63: VK_NUMPAD3
        Keys::Keypad4,         // 0x64: VK_NUMPAD4
        Keys::Keypad5,         // 0x65: VK_NUMPAD5
        Keys::Keypad6,         // 0x66: VK_NUMPAD6
        Keys::Keypad7,         // 0x67: VK_NUMPAD7
        Keys::Keypad8,         // 0x68: VK_NUMPAD8
        Keys::Keypad9,         // 0x69: VK_NUMPAD9
        Keys::KeypadMultiply,  // 0x6A: VK_MULTIPLY
        Keys::KeypadAdd,       // 0x6B: VK_ADD
        Keys::KeypadSeparator, // 0x6C: VK_SEPARATOR
        Keys::KeypadSubtract,  // 0x6D: VK_SUBTRACT
        Keys::KeypadDecimal,   // 0x6E: VK_DECIMAL
        Keys::KeypadDivide,    // 0x6F: VK_DIVIDE
        Keys::F1,              // 0x70: VK_F1
        Keys::F2,              // 0x71: VK_F2
        Keys::F3,              // 0x72: VK_F3
        Keys::F4,              // 0x73: VK_F4
        Keys::F5,              // 0x74: VK_F5
        Keys::F6,              // 0x75: VK_F6
        Keys::F7,              // 0x76: VK_F7
        Keys::F8,              // 0x77: VK_F8
        Keys::F9,              // 0x78: VK_F9
        Keys::F10,             // 0x79: VK_F10
        Keys::F11,             // 0x7A: VK_F11
        Keys::F12,             // 0x7B: VK_F12
        Keys::F13,             // 0x7C: VK_F13
        Keys::F14,             // 0x7D: VK_F14
        Keys::F15,             // 0x7E: VK_F15
        Keys::F16,             // 0x7F: VK_F16
        Keys::F17,             // 0x80: VK_F17
        Keys::F18,             // 0x81: VK_F18
        Keys::F19,             // 0x82: VK_F19
        Keys::F20,             // 0x83: VK_F20
        Keys::F21,             // 0x84: VK_F21
        Keys::F22,             // 0x85: VK_F22
        Keys::F23,             // 0x86: VK_F23
        Keys::F24,             // 0x87: VK_F24
        Keys::Unknown,         // 0x88: VK_NAVIGATION_VIEW
        Keys::Unknown,         // 0x89: VK_NAVIGATION_MENU
        Keys::Unknown,         // 0x8A: VK_NAVIGATION_UP
        Keys::Unknown,         // 0x8B: VK_NAVIGATION_DOWN
        Keys::Unknown,         // 0x8C: VK_NAVIGATION_LEFT
        Keys::Unknown,         // 0x8D: VK_NAVIGATION_RIGHT
        Keys::Unknown,         // 0x8E: VK_NAVIGATION_ACCEPT
        Keys::Unknown,         // 0x8F: VK_NAVIGATION_CANCEL
        Keys::NumLock,         // 0x90: VK_NUMLOCK
        Keys::ScrollLock,      // 0x91: VK_SCROLL
        Keys::Unknown,         // 0x92: VK_OEM_NEC_EQUAL
        Keys::Unknown,         // 0x92: VK_OEM_FJ_JISHO
        Keys::Unknown,         // 0x93: VK_OEM_FJ_MASSHOU
        Keys::Unknown,         // 0x94: VK_OEM_FJ_TOUROKU
        Keys::Unknown,         // 0x95: VK_OEM_FJ_LOYA
        Keys::Unknown,         // 0x96: VK_OEM_FJ_ROYA
        Keys::Unknown,         // 0x97:
        Keys::Unknown,         // 0x98:
        Keys::Unknown,         // 0x99:
        Keys::Unknown,         // 0x9A:
        Keys::Unknown,         // 0x9B:
        Keys::Unknown,         // 0x9C:
        Keys::Unknown,         // 0x9D:
        Keys::Unknown,         // 0x9E:
        Keys::Unknown,         // 0x9F:
        Keys::LeftShift,       // 0xA0: VK_LSHIFT
        Keys::RightControl,    // 0xA1: VK_RSHIFT
        Keys::LeftControl,     // 0xA2: VK_LCONTROL
        Keys::RightControl,    // 0xA3: VK_RCONTROL
        Keys::Unknown,         // 0xA4: VK_LMENU
        Keys::Unknown,         // 0xA5: VK_RMENU
        Keys::Unknown,         // 0xA6: VK_BROWSER_BACK
        Keys::Unknown,         // 0xA7: VK_BROWSER_FORWARD
        Keys::Unknown,         // 0xA8: VK_BROWSER_REFRESH
        Keys::Unknown,         // 0xA9: VK_BROWSER_STOP
        Keys::Unknown,         // 0xAA: VK_BROWSER_SEARCH
        Keys::Unknown,         // 0xAB: VK_BROWSER_FAVORITES
        Keys::Unknown,         // 0xAC: VK_BROWSER_HOME
        Keys::VolumeMute,      // 0xAD: VK_VOLUME_MUTE
        Keys::VolumeDown,      // 0xAE: VK_VOLUME_DOWN
        Keys::VolumeUp,        // 0xAF: VK_VOLUME_UP
        Keys::Unknown,         // 0xB0: VK_MEDIA_NEXT_TRACK
        Keys::Unknown,         // 0xB1: VK_MEDIA_PREV_TRACK
        Keys::Unknown,         // 0xB2: VK_MEDIA_STOP
        Keys::Unknown,         // 0xB3: VK_MEDIA_PLAY_PAUSE
        Keys::Unknown,         // 0xB4: VK_LAUNCH_MAIL
        Keys::Unknown,         // 0xB5: VK_LAUNCH_MEDIA_SELECT
        Keys::Unknown,         // 0xB6: VK_LAUNCH_APP1
        Keys::Unknown,         // 0xB7: VK_LAUNCH_APP2
        Keys::Unknown,         // 0xB8:
        Keys::Unknown,         // 0xB9:
        Keys::Unknown,         // 0xB7:
        Keys::Semicolon,       // 0xBA: VK_OEM_1
        Keys::Plus,            // 0xBB: VK_OEM_PLUS
        Keys::Comma,           // 0xBC: VK_OEM_COMMA
        Keys::Minus,           // 0xBD: VK_OEM_MINUS
        Keys::Period,          // 0xBE: VK_OEM_PERIOD
        Keys::Question,        // 0xBF: VK_OEM_2
        Keys::BackQuate,       // 0xC0: VK_OEM_3
        Keys::Unknown,         // 0xC1:
        Keys::Unknown,         // 0xC2:
        Keys::Unknown,         // 0xC3: VK_GAMEPAD_A
        Keys::Unknown,         // 0xC4: VK_GAMEPAD_B
        Keys::Unknown,         // 0xC5: VK_GAMEPAD_X
        Keys::Unknown,         // 0xC6: VK_GAMEPAD_Y
        Keys::Unknown,         // 0xC7: VK_GAMEPAD_RIGHT_SHOULDER
        Keys::Unknown,         // 0xC8: VK_GAMEPAD_LEFT_SHOULDER
        Keys::Unknown,         // 0xC9: VK_GAMEPAD_LEFT_TRIGGER
        Keys::Unknown,         // 0xCA: VK_GAMEPAD_RIGHT_TRIGGER
        Keys::Unknown,         // 0xCB: VK_GAMEPAD_DPAD_UP
        Keys::Unknown,         // 0xCC: VK_GAMEPAD_DPAD_DOWN
        Keys::Unknown,         // 0xCD: VK_GAMEPAD_DPAD_LEFT
        Keys::Unknown,         // 0xCE: VK_GAMEPAD_DPAD_RIGHT
        Keys::Unknown,         // 0xCF: VK_GAMEPAD_MENU
        Keys::Unknown,         // 0xD0: VK_GAMEPAD_VIEW
        Keys::Unknown,         // 0xD1: VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON
        Keys::Unknown,         // 0xD2: VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON
        Keys::Unknown,         // 0xD3: VK_GAMEPAD_LEFT_THUMBSTICK_UP
        Keys::Unknown,         // 0xD4: VK_GAMEPAD_LEFT_THUMBSTICK_DOWN
        Keys::Unknown,         // 0xD5: VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT
        Keys::Unknown,         // 0xD6: VK_GAMEPAD_LEFT_THUMBSTICK_LEFT
        Keys::Unknown,         // 0xD7: VK_GAMEPAD_RIGHT_THUMBSTICK_UP
        Keys::Unknown,         // 0xD8: VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN
        Keys::Unknown,         // 0xD9: VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT
        Keys::Unknown,         // 0xDA: VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT
        Keys::OpenBracket,     // 0xDB: VK_OEM_4
        Keys::Backslash,       // 0xDC: VK_OEM_5
        Keys::CloseBracket,    // 0xDD: VK_OEM_6
        Keys::Quote,           // 0xDE: VK_OEM_7
        Keys::Unknown,         // 0xDF: VK_OEM_8
        Keys::Unknown,         // 0xE0:
        Keys::Unknown,         // 0xE1: VK_OEM_AX
        Keys::Backslash,       // 0xE2: VK_OEM_102
    };

    if (const auto index = static_cast<size_t>(keyCode - 1); index < keyTable.size()) {
        return keyTable[index];
    }
    return Keys::Unknown;
}

} // namespace

void KeyboardWin32::HandleMessage(const SystemEvent& event)
{
    switch (event.Kind) {
    case SystemEventKind::InputKeyEvent: {
        const auto ev = std::get<InputKeyEvent>(event.Data);

        const bool isKeyDown = keyboardState.IsKeyDown(ev.Key);
        keyboardState.SetKey(ev.Key, ev.State);

        switch (ev.State) {
        case KeyState::Down:
            if (!isKeyDown) {
                Keyboard::KeyDown(ev.Key);
            }
            break;
        case KeyState::Up:
            if (isKeyDown) {
                Keyboard::KeyUp(ev.Key);
            }
            break;
        }
        break;
    }
    case SystemEventKind::InputTextEvent: {
        const auto& ev = std::get<InputTextEvent>(event.Data);
        Keyboard::TextInput(ev.Text);
        break;
    }
    default:
        break;
    }
}

KeyboardState KeyboardWin32::GetState() const
{
    return keyboardState;
}

void TranslateKeyboardEvent(const RAWKEYBOARD& keyboard, const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue) noexcept
{
    const auto key = TranslateKey(keyboard.VKey);
    if (key == Keys::Unknown) {
        return;
    }

    const auto keyState = (keyboard.Flags & RI_KEY_BREAK ? KeyState::Up : KeyState::Down);
    eventQueue->Enqueue(SystemEvent{
        .Kind = SystemEventKind::InputKeyEvent,
        .Data = InputKeyEvent{
            .State = keyState,
            .Key = *key,
        },
    });
}

} // namespace pomdog::detail::win32
