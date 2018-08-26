// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Input/Keys.hpp"
#include "KeyboardX11.hpp"
#include "../Utility/ScopeGuard.hpp"
#include "Pomdog/Basic/Platform.hpp"
#include "Pomdog/Input/KeyState.hpp"
#include "Pomdog/Logging/Log.hpp"
#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <algorithm>
#include <cstring>
#include <map>

namespace Pomdog {
namespace Detail {
namespace X11 {
namespace {

constexpr Keys Keys_None = static_cast<Keys>(0);

Keys TranslateKey(Display* display, unsigned int keyCode)
{
    if (keyCode < 8 || keyCode > 255) {
        return Keys_None;
    }

    auto keySym = XkbKeycodeToKeysym(display, keyCode, 0, 1);

    switch (keySym) {
    case XK_KP_0: return Keys::Keypad0;
    case XK_KP_1: return Keys::Keypad1;
    case XK_KP_2: return Keys::Keypad2;
    case XK_KP_3: return Keys::Keypad3;
    case XK_KP_4: return Keys::Keypad4;
    case XK_KP_5: return Keys::Keypad5;
    case XK_KP_6: return Keys::Keypad6;
    case XK_KP_7: return Keys::Keypad7;
    case XK_KP_8: return Keys::Keypad8;
    case XK_KP_9: return Keys::Keypad9;
    case XK_KP_Enter: return Keys::Enter;
    case XK_KP_Equal: return Keys::Equals;
    case XK_KP_Separator: return Keys::KeypadDecimal;
    case XK_KP_Decimal: return Keys::KeypadDecimal;
    default:
        break;
    }

    keySym = XkbKeycodeToKeysym(display, keyCode, 0, 0);

    static_assert(XK_0 == '0' && XK_9 == '9' && XK_0 < XK_9, "");
    static_assert(XK_A == 'A' && XK_Z == 'Z' && XK_A < XK_Z, "");
    static_assert(XK_a == 'a' && XK_z == 'z' && XK_a < XK_z, "");

    if (keySym >= '0' && keySym <= '9') {
        return static_cast<Keys>(
            static_cast<std::uint8_t>(Keys::Alpha0) +
            static_cast<std::uint8_t>(keySym - '0'));
    }

    if (keySym >= 'A' && keySym <= 'Z') {
        return static_cast<Keys>(
            static_cast<std::uint8_t>(Keys::A) +
            static_cast<std::uint8_t>(keySym - 'A'));
    }

    if (keySym >= 'a' && keySym <= 'z') {
        return static_cast<Keys>(
            static_cast<std::uint8_t>(Keys::A) +
            static_cast<std::uint8_t>(keySym - 'a'));
    }

    switch (keySym) {
    case XK_BackSpace: return Keys::BackSpace;
    case XK_Tab: return Keys::Tab;
    case XK_Return: return Keys::Enter;
    case XK_Escape: return Keys::Escape;
    case XK_Delete: return Keys::Delete;
    case XK_Pause: return Keys::Pause;
    case XK_Num_Lock: return Keys::NumLock;
    case XK_Caps_Lock: return Keys::CapsLock;
    case XK_Scroll_Lock: return Keys::ScrollLock;
    case XK_Print: return Keys::Print;
    case XK_Menu: return Keys::Help;
    case XK_Home: return Keys::Home;
    case XK_End: return Keys::Enter;
    case XK_Page_Up: return Keys::PageUp;
    case XK_Page_Down: return Keys::PageDown;
    case XK_Insert: return Keys::Insert;
    case XK_Left: return Keys::LeftArrow;
    case XK_Right: return Keys::RightArrow;
    case XK_Down: return Keys::DownArrow;
    case XK_Up: return Keys::UpArrow;
    case XK_Shift_L: return Keys::LeftShift;
    case XK_Shift_R: return Keys::RightShift;
    case XK_Control_L: return Keys::LeftControl;
    case XK_Control_R: return Keys::RightControl;
    case XK_Alt_L: return Keys::LeftAlt;
    case XK_Alt_R: return Keys::RightAlt;
    case XK_Super_L: return Keys::LeftWindows;
    case XK_Super_R: return Keys::RightWindows;
#if defined(POMDOG_PLATFORM_MACOSX)
    case XK_Meta_L: return Keys::LeftApple;
    case XK_Meta_R: return Keys::RightApple;
#endif

    case XK_F1: return Keys::F1;
    case XK_F2: return Keys::F2;
    case XK_F3: return Keys::F3;
    case XK_F4: return Keys::F4;
    case XK_F5: return Keys::F5;
    case XK_F6: return Keys::F6;
    case XK_F7: return Keys::F7;
    case XK_F8: return Keys::F8;
    case XK_F9: return Keys::F9;
    case XK_F10: return Keys::F10;
    case XK_F11: return Keys::F11;
    case XK_F12: return Keys::F12;
    case XK_F13: return Keys::F13;
    case XK_F14: return Keys::F14;
    case XK_F15: return Keys::F15;

    case XK_KP_Add: return Keys::KeypadAdd;
    case XK_KP_Divide: return Keys::KeypadDivide;
    case XK_KP_Multiply: return Keys::KeypadMultiply;
    case XK_KP_Subtract: return Keys::KeypadSubtract;
    case XK_KP_Insert: return Keys::Keypad0;
    case XK_KP_End: return Keys::Keypad1;
    case XK_KP_Down: return Keys::Keypad2;
    case XK_KP_Page_Down: return Keys::Keypad3;
    case XK_KP_Left: return Keys::Keypad4;
    case XK_KP_Right: return Keys::Keypad6;
    case XK_KP_Home: return Keys::Keypad7;
    case XK_KP_Up: return Keys::Keypad8;
    case XK_KP_Page_Up: return Keys::Keypad9;
    case XK_KP_Delete: return Keys::KeypadDecimal;
    case XK_KP_F1: return Keys::F1;
    case XK_KP_F2: return Keys::F2;
    case XK_KP_F3: return Keys::F3;
    case XK_KP_F4: return Keys::F4;
    case XK_KP_Equal: return Keys::Equals;
    case XK_KP_Enter: return Keys::Enter;

    case XK_space: return Keys::Space;
    case XK_braceleft: return Keys::OpenBracket;
    case XK_braceright: return Keys::CloseBracket;
    case XK_semicolon: return Keys::Semicolon;
    //case XK_colon: return Keys::Colon;
    case XK_question: return Keys::Question;
    case XK_backslash: return Keys::Backslash;
    case XK_equal: return Keys::Equals;
    case XK_apostrophe: return Keys::Quote;
    case XK_grave: return Keys::AccentGrave;
    case XK_comma: return Keys::Comma;
    case XK_period: return Keys::Period;
    case XK_slash: return Keys::Slash;
    case XK_plus: return Keys::Plus;
    case XK_minus: return Keys::Minus;

    case XK_Mode_switch: return Keys::RightAlt;
    //case XK_ISO_Level3_Shift: return Keys::RightAlt;
    //case XK_Kanji: return Keys::KanjiMode;
    //case XK_kana_switch: return Keys::KanaMode;

    default:
        break;
    }

    return Keys_None;
}

void BuildKeyMap(Display* display, std::array<Keys, 256> & keys)
{
    std::fill(std::begin(keys), std::end(keys), Keys_None);

    std::array<char, XkbKeyNameLength + 1> name;
    std::fill(std::begin(name), std::end(name), 0);

    XkbDescPtr desc = XkbGetKeyboard(
        display, XkbAllComponentsMask, XkbUseCoreKbd);

    Detail::ScopeGuard scoped([&] {
        XkbFreeKeyboard(desc, 0, True);
    });

    std::map<std::string, Keys> const keyCodes = {
        {"AE01", Keys::Alpha1},
        {"AE02", Keys::Alpha2},
        {"AE03", Keys::Alpha3},
        {"AE04", Keys::Alpha4},
        {"AE05", Keys::Alpha5},
        {"AE06", Keys::Alpha6},
        {"AE07", Keys::Alpha7},
        {"AE08", Keys::Alpha8},
        {"AE09", Keys::Alpha9},
        {"AE10", Keys::Alpha0},
        {"AE11", Keys::Minus},
        {"AE12", Keys::Equals},
        {"AD01", Keys::Q},
        {"AD02", Keys::W},
        {"AD03", Keys::E},
        {"AD04", Keys::R},
        {"AD05", Keys::T},
        {"AD06", Keys::Y},
        {"AD07", Keys::U},
        {"AD08", Keys::I},
        {"AD09", Keys::O},
        {"AD10", Keys::P},
        {"AD11", Keys::OpenBracket},
        {"AD12", Keys::CloseBracket},
        {"AC01", Keys::A},
        {"AC02", Keys::S},
        {"AC03", Keys::D},
        {"AC04", Keys::F},
        {"AC05", Keys::G},
        {"AC06", Keys::H},
        {"AC07", Keys::J},
        {"AC08", Keys::K},
        {"AC09", Keys::L},
        {"AC10", Keys::Semicolon},
        {"AC11", Keys::Quote},
        {"AB01", Keys::Z},
        {"AB02", Keys::X},
        {"AB03", Keys::C},
        {"AB04", Keys::V},
        {"AB05", Keys::B},
        {"AB06", Keys::N},
        {"AB07", Keys::M},
        {"AB08", Keys::Comma},
        {"AB09", Keys::Period},
        {"AB10", Keys::Slash},
        {"BKSL", Keys::Backslash},
        {"SPCE", Keys::Space},
        {"UP", Keys::UpArrow},
        {"DOWN", Keys::DownArrow},
        {"LEFT", Keys::LeftArrow},
        {"RIGHT", Keys::RightArrow},
    };

    POMDOG_ASSERT(desc->min_key_code <= desc->max_key_code);
    for (int keyCode = desc->min_key_code; keyCode <= desc->max_key_code; ++keyCode) {
        std::memcpy(name.data(), desc->names->keys[keyCode].name, XkbKeyNameLength);
        POMDOG_ASSERT(name.back() == 0);

        auto iter = keyCodes.find(name.data());
        if (iter != std::end(keyCodes)) {
            if (keyCode >= 0 && keyCode < static_cast<int>(keys.size())) {
                keys[keyCode] = iter->second;
            }
//            static_assert(std::numeric_limits<decltype(keyCode)>::min() >= 0, "");
//            if (keyCode < keys.size()) {
//                keys[keyCode] = iter->second;
//            }
        }
    }

    for (unsigned int keyCode = 0; keyCode < keys.size(); ++keyCode) {
        if (keys[keyCode] == Keys_None) {
            keys[keyCode] = TranslateKey(display, keyCode);
        }
    }
}

Keys ToKeys(const std::array<Keys, 256>& keys, unsigned int keyCode)
{
    if (keyCode < 8 || keyCode > keys.size()) {
        return Keys_None;
    }

    POMDOG_ASSERT(keyCode < keys.size());
    return keys[keyCode];
}

} // unnamed namespace

KeyboardX11::KeyboardX11(::Display* display)
{
    POMDOG_ASSERT(display != nullptr);
    BuildKeyMap(display, mappedKeys);
}

KeyboardState KeyboardX11::GetState() const
{
    return keyboardState;
}

void KeyboardX11::HandleEvent(XEvent & event)
{
    if (event.type != KeyPress && event.type != KeyRelease) {
        return;
    }

    auto key = ToKeys(mappedKeys, event.xkey.keycode);
    auto keyState = (event.type == KeyPress ? KeyState::Down: KeyState::Up);

    if (key == Keys_None) {
        return;
    }

    bool isKeyDown = keyboardState.IsKeyDown(key);
    keyboardState.SetKey(key, keyState);

    switch (keyState) {
    case KeyState::Down:
        if (!isKeyDown) {
            Keyboard::KeyDown(key);
        }
        break;
    case KeyState::Up:
        if (isKeyDown) {
            Keyboard::KeyUp(key);
        }
        break;
    }

#if defined(DEBUG) && !defined(NDEBUG)
    if (key == Keys_None) {
        Log::Internal(std::string("IsKeyDown: Unspecified key: ")
            + std::to_string(static_cast<int>(event.xkey.keycode)));
    }
#endif
}

} // namespace X11
} // namespace Detail
} // namespace Pomdog
