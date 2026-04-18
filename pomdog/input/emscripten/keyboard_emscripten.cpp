// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/emscripten/keyboard_emscripten.h"
#include "pomdog/application/backends/system_event_queue.h"
#include "pomdog/application/backends/system_events.h"
#include "pomdog/input/backends/keyboard_impl.h"
#include "pomdog/input/key_state.h"
#include "pomdog/input/keys.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <array>
#include <cstring>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::emscripten {
namespace {

[[nodiscard]] Keys
translateKeyFromKeyCode(unsigned long keyCode) noexcept
{
    // NOTE: Reference https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/keyCode
    constexpr std::array<Keys, 223> keyTable = {
        Keys::Unknown,         // 0x00:
        Keys::Unknown,         // 0x01:
        Keys::Unknown,         // 0x02:
        Keys::Unknown,         // 0x03: DOM_VK_CANCEL
        Keys::Unknown,         // 0x04:
        Keys::Unknown,         // 0x05:
        Keys::Help,            // 0x06: DOM_VK_HELP
        Keys::Unknown,         // 0x07:
        Keys::BackSpace,       // 0x08: DOM_VK_BACK_SPACE
        Keys::Tab,             // 0x09: DOM_VK_TAB
        Keys::Unknown,         // 0x0A:
        Keys::Unknown,         // 0x0B:
        Keys::Clear,           // 0x0C: DOM_VK_CLEAR
        Keys::Enter,           // 0x0D: DOM_VK_RETURN
        Keys::Enter,           // 0x0E: DOM_VK_ENTER
        Keys::Unknown,         // 0x0F:
        Keys::LeftShift,       // 0x10: DOM_VK_SHIFT
        Keys::LeftControl,     // 0x11: DOM_VK_CONTROL
        Keys::LeftAlt,         // 0x12: DOM_VK_ALT
        Keys::Pause,           // 0x13: DOM_VK_PAUSE
        Keys::CapsLock,        // 0x14: DOM_VK_CAPS_LOCK
        Keys::KanaMode,        // 0x15: DOM_VK_KANA
        Keys::Unknown,         // 0x16: DOM_VK_EISU
        Keys::JunjaMode,       // 0x17: DOM_VK_JUNJA
        Keys::Unknown,         // 0x18: DOM_VK_FINAL
        Keys::KanjiMode,       // 0x19: DOM_VK_KANJI
        Keys::Unknown,         // 0x1A:
        Keys::Escape,          // 0x1B: DOM_VK_ESCAPE
        Keys::Convert,         // 0x1C: DOM_VK_CONVERT
        Keys::NonConvert,      // 0x1D: DOM_VK_NONCONVERT
        Keys::Unknown,         // 0x1E: DOM_VK_ACCEPT
        Keys::Unknown,         // 0x1F: DOM_VK_MODECHANGE
        Keys::Space,           // 0x20: DOM_VK_SPACE
        Keys::PageUp,          // 0x21: DOM_VK_PAGE_UP
        Keys::PageDown,        // 0x22: DOM_VK_PAGE_DOWN
        Keys::End,             // 0x23: DOM_VK_END
        Keys::Home,            // 0x24: DOM_VK_HOME
        Keys::LeftArrow,       // 0x25: DOM_VK_LEFT
        Keys::UpArrow,         // 0x26: DOM_VK_UP
        Keys::RightArrow,      // 0x27: DOM_VK_RIGHT
        Keys::DownArrow,       // 0x28: DOM_VK_DOWN
        Keys::Select,          // 0x29: DOM_VK_SELECT
        Keys::Print,           // 0x2A: DOM_VK_PRINT
        Keys::Execute,         // 0x2B: DOM_VK_EXECUTE
        Keys::PrintScreen,     // 0x2C: DOM_VK_PRINTSCREEN
        Keys::Insert,          // 0x2D: DOM_VK_INSERT
        Keys::Delete,          // 0x2E: DOM_VK_DELETE
        Keys::Unknown,         // 0x2F:
        Keys::Digit0,          // 0x30: DOM_VK_0
        Keys::Digit1,          // 0x31: DOM_VK_1
        Keys::Digit2,          // 0x32: DOM_VK_2
        Keys::Digit3,          // 0x33: DOM_VK_3
        Keys::Digit4,          // 0x34: DOM_VK_4
        Keys::Digit5,          // 0x35: DOM_VK_5
        Keys::Digit6,          // 0x36: DOM_VK_6
        Keys::Digit7,          // 0x37: DOM_VK_7
        Keys::Digit8,          // 0x38: DOM_VK_8
        Keys::Digit9,          // 0x39: DOM_VK_9
        Keys::Unknown,         // 0x3A: DOM_VK_COLON
        Keys::Semicolon,       // 0x3B: DOM_VK_SEMICOLON
        Keys::Unknown,         // 0x3C: DOM_VK_LESS_THAN
        Keys::Equals,          // 0x3D: DOM_VK_EQUALS
        Keys::Unknown,         // 0x3E: DOM_VK_GREATER_THAN
        Keys::Question,        // 0x3F: DOM_VK_QUESTION_MARK
        Keys::Unknown,         // 0x40: DOM_VK_AT
        Keys::A,               // 0x41: DOM_VK_A
        Keys::B,               // 0x42: DOM_VK_B
        Keys::C,               // 0x43: DOM_VK_C
        Keys::D,               // 0x44: DOM_VK_D
        Keys::E,               // 0x45: DOM_VK_E
        Keys::F,               // 0x46: DOM_VK_F
        Keys::G,               // 0x47: DOM_VK_G
        Keys::H,               // 0x48: DOM_VK_H
        Keys::I,               // 0x49: DOM_VK_I
        Keys::J,               // 0x4A: DOM_VK_J
        Keys::K,               // 0x4B: DOM_VK_K
        Keys::L,               // 0x4C: DOM_VK_L
        Keys::M,               // 0x4D: DOM_VK_M
        Keys::N,               // 0x4E: DOM_VK_N
        Keys::O,               // 0x4F: DOM_VK_O
        Keys::P,               // 0x50: DOM_VK_P
        Keys::Q,               // 0x51: DOM_VK_Q
        Keys::R,               // 0x52: DOM_VK_R
        Keys::S,               // 0x53: DOM_VK_S
        Keys::T,               // 0x54: DOM_VK_T
        Keys::U,               // 0x55: DOM_VK_U
        Keys::V,               // 0x56: DOM_VK_V
        Keys::W,               // 0x57: DOM_VK_W
        Keys::X,               // 0x58: DOM_VK_X
        Keys::Y,               // 0x59: DOM_VK_Y
        Keys::Z,               // 0x5A: DOM_VK_Z
        Keys::LeftSuper,       // 0x5B: DOM_VK_WIN
        Keys::RightSuper,      // 0x5C: MetaRight
        Keys::ContextMenu,     // 0x5D: DOM_VK_CONTEXT_MENU
        Keys::Unknown,         // 0x5E:
        Keys::Sleep,           // 0x5F: DOM_VK_SLEEP
        Keys::Numpad0,         // 0x60: DOM_VK_NUMPAD0
        Keys::Numpad1,         // 0x61: DOM_VK_NUMPAD1
        Keys::Numpad2,         // 0x62: DOM_VK_NUMPAD2
        Keys::Numpad3,         // 0x63: DOM_VK_NUMPAD3
        Keys::Numpad4,         // 0x64: DOM_VK_NUMPAD4
        Keys::Numpad5,         // 0x65: DOM_VK_NUMPAD5
        Keys::Numpad6,         // 0x66: DOM_VK_NUMPAD6
        Keys::Numpad7,         // 0x67: DOM_VK_NUMPAD7
        Keys::Numpad8,         // 0x68: DOM_VK_NUMPAD8
        Keys::Numpad9,         // 0x69: DOM_VK_NUMPAD9
        Keys::NumpadMultiply,  // 0x6A: DOM_VK_MULTIPLY
        Keys::NumpadAdd,       // 0x6B: DOM_VK_ADD
        Keys::NumpadSeparator, // 0x6C: DOM_VK_SEPARATOR
        Keys::NumpadSubtract,  // 0x6D: DOM_VK_SUBTRACT
        Keys::NumpadDecimal,   // 0x6E: DOM_VK_DECIMAL
        Keys::NumpadDivide,    // 0x6F: DOM_VK_DIVIDE
        Keys::F1,              // 0x70: DOM_VK_F1
        Keys::F2,              // 0x71: DOM_VK_F2
        Keys::F3,              // 0x72: DOM_VK_F3
        Keys::F4,              // 0x73: DOM_VK_F4
        Keys::F5,              // 0x74: DOM_VK_F5
        Keys::F6,              // 0x75: DOM_VK_F6
        Keys::F7,              // 0x76: DOM_VK_F7
        Keys::F8,              // 0x77: DOM_VK_F8
        Keys::F9,              // 0x78: DOM_VK_F9
        Keys::F10,             // 0x79: DOM_VK_F10
        Keys::F11,             // 0x7A: DOM_VK_F11
        Keys::F12,             // 0x7B: DOM_VK_F12
        Keys::F13,             // 0x7C: DOM_VK_F13
        Keys::F14,             // 0x7D: DOM_VK_F14
        Keys::F15,             // 0x7E: DOM_VK_F15
        Keys::F16,             // 0x7F: DOM_VK_F16
        Keys::F17,             // 0x80: DOM_VK_F17
        Keys::F18,             // 0x81: DOM_VK_F18
        Keys::F19,             // 0x82: DOM_VK_F19
        Keys::F20,             // 0x83: DOM_VK_F20
        Keys::F21,             // 0x84: DOM_VK_F21
        Keys::F22,             // 0x85: DOM_VK_F22
        Keys::F23,             // 0x86: DOM_VK_F23
        Keys::F24,             // 0x87: DOM_VK_F24
        Keys::Unknown,         // 0x88:
        Keys::Unknown,         // 0x89:
        Keys::Unknown,         // 0x8A:
        Keys::Unknown,         // 0x8B:
        Keys::Unknown,         // 0x8C:
        Keys::Unknown,         // 0x8D:
        Keys::Unknown,         // 0x8E:
        Keys::Unknown,         // 0x8F:
        Keys::NumLock,         // 0x90: DOM_VK_NUM_LOCK
        Keys::ScrollLock,      // 0x91: DOM_VK_SCROLL_LOCK
        Keys::Unknown,         // 0x92:
        Keys::Unknown,         // 0x93:
        Keys::Unknown,         // 0x94:
        Keys::Unknown,         // 0x95:
        Keys::Unknown,         // 0x96:
        Keys::Unknown,         // 0x97:
        Keys::Unknown,         // 0x98:
        Keys::Unknown,         // 0x99:
        Keys::Unknown,         // 0x9A:
        Keys::Unknown,         // 0x9B:
        Keys::Unknown,         // 0x9C:
        Keys::Unknown,         // 0x9D:
        Keys::Unknown,         // 0x9E:
        Keys::Unknown,         // 0x9F:
        Keys::Unknown,         // 0xA0:
        Keys::Unknown,         // 0xA1:
        Keys::DoubleQuote,     // 0xA2: DOM_VK_DOUBLE_QUOTE
        Keys::Unknown,         // 0xA3:
        Keys::Unknown,         // 0xA4:
        Keys::Unknown,         // 0xA5:
        Keys::Unknown,         // 0xA6:
        Keys::Unknown,         // 0xA7:
        Keys::Unknown,         // 0xA8:
        Keys::Unknown,         // 0xA9:
        Keys::NumpadMultiply,  // 0xAA: DOM_VK_ASTERISK
        Keys::Plus,            // 0xAB: DOM_VK_PLUS
        Keys::Unknown,         // 0xAC:
        Keys::Minus,           // 0xAD: DOM_VK_HYPHEN_MINUS
        Keys::Unknown,         // 0xAE:
        Keys::Unknown,         // 0xAF:
        Keys::Tilde,           // 0xB0: DOM_VK_TILDE
        Keys::Unknown,         // 0xB1:
        Keys::Unknown,         // 0xB2:
        Keys::Unknown,         // 0xB3:
        Keys::Unknown,         // 0xB4:
        Keys::AudioVolumeMute, // 0xB5: DOM_VK_VOLUME_MUTE
        Keys::AudioVolumeDown, // 0xB6: DOM_VK_VOLUME_DOWN
        Keys::AudioVolumeUp,   // 0xB7: DOM_VK_VOLUME_UP
        Keys::Unknown,         // 0xB8:
        Keys::Unknown,         // 0xB9:
        Keys::Semicolon,       // 0xBA: Semicolon on Chrome/Safari
        Keys::Equals,          // 0xBB: Equal on Chrome/Safari
        Keys::Comma,           // 0xBC: DOM_VK_COMMA
        Keys::Minus,           // 0xBD: Minus on Chrome/Safari
        Keys::Period,          // 0xBE: DOM_VK_PERIOD
        Keys::Slash,           // 0xBF: DOM_VK_SLASH
        Keys::Backquote,       // 0xC0: DOM_VK_BACK_QUOTE
        Keys::Unknown,         // 0xC1:
        Keys::Unknown,         // 0xC2:
        Keys::Unknown,         // 0xC3:
        Keys::Unknown,         // 0xC4:
        Keys::Unknown,         // 0xC5:
        Keys::Unknown,         // 0xC6:
        Keys::Unknown,         // 0xC7:
        Keys::Unknown,         // 0xC8:
        Keys::Unknown,         // 0xC9:
        Keys::Unknown,         // 0xCA:
        Keys::Unknown,         // 0xCB:
        Keys::Unknown,         // 0xCC:
        Keys::Unknown,         // 0xCD:
        Keys::Unknown,         // 0xCE:
        Keys::Unknown,         // 0xCF:
        Keys::Unknown,         // 0xD0:
        Keys::Unknown,         // 0xD1:
        Keys::Unknown,         // 0xD2:
        Keys::Unknown,         // 0xD3:
        Keys::Unknown,         // 0xD4:
        Keys::Unknown,         // 0xD5:
        Keys::Unknown,         // 0xD6:
        Keys::Unknown,         // 0xD7:
        Keys::Unknown,         // 0xD8:
        Keys::Unknown,         // 0xD9:
        Keys::Unknown,         // 0xDA:
        Keys::OpenBracket,     // 0xDB: DOM_VK_OPEN_BRACKET
        Keys::Backslash,       // 0xDC: DOM_VK_BACK_SLASH
        Keys::CloseBracket,    // 0xDD: DOM_VK_CLOSE_BRACKET
        Keys::Quote,           // 0xDE: DOM_VK_QUOTE
    };

    if (keyCode < keyTable.size()) {
        return keyTable[keyCode];
    }
    return Keys::Unknown;
}

[[nodiscard]] Keys
translateKey(
    const EM_UTF8 code[EM_HTML5_SHORT_STRING_LEN_BYTES],
    unsigned long keyCode) noexcept
{
    // NOTE: Try code field first (more reliable than keyCode)
    switch (code[0]) {
    case 'B':
        if (std::strcmp(code, "Backquote") == 0) {
            return Keys::Backquote;
        }
        if (std::strcmp(code, "Backslash") == 0) {
            return Keys::Backslash;
        }
        if (std::strcmp(code, "BracketLeft") == 0) {
            return Keys::OpenBracket;
        }
        if (std::strcmp(code, "BracketRight") == 0) {
            return Keys::CloseBracket;
        }
        break;
    case 'E':
        if (std::strcmp(code, "Equal") == 0) {
            return Keys::Equals;
        }
        break;
    case 'H':
        if (std::strcmp(code, "Help") == 0) {
            return Keys::Help;
        }
        break;
    case 'M':
        if (std::strcmp(code, "Minus") == 0) {
            return Keys::Minus;
        }
        if (std::strcmp(code, "MetaLeft") == 0) {
            return Keys::LeftSuper;
        }
        if (std::strcmp(code, "MetaRight") == 0) {
            return Keys::RightSuper;
        }
        break;
    case 'P':
        if (std::strcmp(code, "Pause") == 0) {
            return Keys::Pause;
        }
        if (std::strcmp(code, "PrintScreen") == 0) {
            return Keys::PrintScreen;
        }
        break;
    case 'Q':
        if (std::strcmp(code, "Quote") == 0) {
            return Keys::Quote;
        }
        break;
    case 'S':
        if (std::strcmp(code, "Semicolon") == 0) {
            return Keys::Semicolon;
        }
        break;
    default:
        break;
    }

    // NOTE: Fallback to keyCode translation
    return translateKeyFromKeyCode(keyCode);
}

[[nodiscard]] EM_BOOL
handleKeyboardEvent(int eventType, const EmscriptenKeyboardEvent* e, void* userData)
{
    auto* eventQueue = reinterpret_cast<SystemEventQueue*>(userData);
    if (eventQueue == nullptr) {
        return EM_FALSE;
    }

    const auto key = translateKey(e->code, e->keyCode);
    if (key == Keys::Unknown) {
        return EM_TRUE;
    }

    const auto keyState = (eventType == EMSCRIPTEN_EVENT_KEYUP)
                              ? KeyState::Up
                              : KeyState::Down;

    eventQueue->enqueue(SystemEvent{
        .kind = SystemEventKind::InputKeyEvent,
        .data = InputKeyEvent{
            .state = keyState,
            .key = key,
        },
    });

    // NOTE: Capture text input from the key field on keydown events.
    if (eventType == EMSCRIPTEN_EVENT_KEYDOWN) {
        const auto* keyStr = e->key;
        // Only treat single-character strings as text input (printable characters).
        if (keyStr[0] != '\0' && keyStr[1] == '\0') {
            eventQueue->enqueue(SystemEvent{
                .kind = SystemEventKind::InputTextEvent,
                .data = InputTextEvent{
                    .text = std::string(keyStr),
                },
            });
        }
    }

    return EM_TRUE;
}

} // namespace

KeyboardEmscripten::KeyboardEmscripten(const std::shared_ptr<KeyboardImpl>& impl) noexcept
    : impl_(impl)
{
    POMDOG_ASSERT(impl_ != nullptr);
}

KeyboardEmscripten::~KeyboardEmscripten() noexcept
{
    unsubscribeEvent();
}

void KeyboardEmscripten::subscribeEvent(const std::shared_ptr<SystemEventQueue>& eventQueue) noexcept
{
    auto* eq = eventQueue.get();
    emscripten_set_keydown_callback(
        EMSCRIPTEN_EVENT_TARGET_WINDOW, eq, EM_TRUE, handleKeyboardEvent);
    emscripten_set_keyup_callback(
        EMSCRIPTEN_EVENT_TARGET_WINDOW, eq, EM_TRUE, handleKeyboardEvent);
}

void KeyboardEmscripten::unsubscribeEvent() noexcept
{
    emscripten_set_keydown_callback(
        EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_TRUE, nullptr);
    emscripten_set_keyup_callback(
        EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_TRUE, nullptr);
}

void KeyboardEmscripten::handleMessage(const SystemEvent& event)
{
    POMDOG_ASSERT(impl_ != nullptr);

    switch (event.kind) {
    case SystemEventKind::InputKeyEvent: {
        const auto ev = std::get<InputKeyEvent>(event.data);
        impl_->setKey(ev.key, ev.state);
        break;
    }
    case SystemEventKind::InputTextEvent: {
        const auto& ev = std::get<InputTextEvent>(event.data);
        impl_->appendTextInput(ev.text);
        break;
    }
    default:
        break;
    }
}

} // namespace pomdog::detail::emscripten
