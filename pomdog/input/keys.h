// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Keys specifies keyboard keys and key codes.
enum class Keys : std::uint8_t {
    Unknown = 0,     ///< Reserved (never is pressed)
    Cancel,          ///< Cancel key.
    Help,            ///< Help key.
    BackSpace,       ///< BackSpace key.
    Tab,             ///< Tab key.
    Clear,           ///< Clear key.
    Enter,           ///< Enter key.
    LeftShift,       ///< Left Shift key.
    RightShift,      ///< Right Shift key.
    LeftControl,     ///< Left Control key.
    RightControl,    ///< Right Control key.
    LeftAlt,         ///< Left Alt key.
    RightAlt,        ///< Right Alt key.
    Pause,           ///< Pause key.
    CapsLock,        ///< Caps lock key.
    KanaMode,        ///< IME Kana mode key.
    HangulMode,      ///< IME Hangul mode key.
    JunjaMode,       ///< IME Junja mode key.
    FinalMode,       ///< IME final mode key.
    KanjiMode,       ///< IME Kanji mode key.
    Escape,          ///< Escape key.
    IMEConvert,      ///< IME Convert key.
    IMENonConvert,   ///< IME NonConvert key.
    Accept,          ///< Accept key.
    ModeChange,      ///< ModeChange key.
    Space,           ///< Space key.
    PageUp,          ///< Page up key.
    PageDown,        ///< Page down key.
    End,             ///< End key.
    Home,            ///< Home key.
    LeftArrow,       ///< Left arrow key.
    UpArrow,         ///< Up arrow key.
    RightArrow,      ///< Right arrow key.
    DownArrow,       ///< Down arrow key.
    Select,          ///< Select key.
    Print,           ///< Print key.
    Execute,         ///< Execute key.
    PrintScreen,     ///< PrintScreen key.
    Insert,          ///< Insert key.
    Delete,          ///< Delete key.
    Alpha0,          ///< '0' key.
    Alpha1,          ///< '1' key.
    Alpha2,          ///< '2' key.
    Alpha3,          ///< '3' key.
    Alpha4,          ///< '4' key.
    Alpha5,          ///< '5' key.
    Alpha6,          ///< '6' key.
    Alpha7,          ///< '7' key.
    Alpha8,          ///< '8' key.
    Alpha9,          ///< '9' key.
    Colon,           ///< ':' key.
    Semicolon,       ///< ';' key.
    Equals,          ///< '=' key.
    Question,        ///< '?' key.
    A,               ///< Letter 'A' key.
    B,               ///< Letter 'B' key.
    C,               ///< Letter 'C' key.
    D,               ///< Letter 'D' key.
    E,               ///< Letter 'E' key.
    F,               ///< Letter 'F' key.
    G,               ///< Letter 'G' key.
    H,               ///< Letter 'H' key.
    I,               ///< Letter 'I' key.
    J,               ///< Letter 'J' key.
    K,               ///< Letter 'K' key.
    L,               ///< Letter 'L' key.
    M,               ///< Letter 'M' key.
    N,               ///< Letter 'N' key.
    O,               ///< Letter 'O' key.
    P,               ///< Letter 'P' key.
    Q,               ///< Letter 'Q' key.
    R,               ///< Letter 'R' key.
    S,               ///< Letter 'S' key.
    T,               ///< Letter 'T' key.
    U,               ///< Letter 'U' key.
    V,               ///< Letter 'V' key.
    W,               ///< Letter 'W' key.
    X,               ///< Letter 'X' key.
    Y,               ///< Letter 'Y' key.
    Z,               ///< Letter 'Z' key.
    LeftSuper,       ///< Left Apple/Windows key.
    RightSuper,      ///< Right Apple/Windows key.
    Applications,    ///< Applications key.
    Sleep,           ///< Sleep key.
    Keypad0,         ///< '0' key on the numeric keypad.
    Keypad1,         ///< '1' key on the numeric keypad.
    Keypad2,         ///< '2' key on the numeric keypad.
    Keypad3,         ///< '3' key on the numeric keypad.
    Keypad4,         ///< '4' key on the numeric keypad.
    Keypad5,         ///< '5' key on the numeric keypad.
    Keypad6,         ///< '6' key on the numeric keypad.
    Keypad7,         ///< '7' key on the numeric keypad.
    Keypad8,         ///< '8' key on the numeric keypad.
    Keypad9,         ///< '9' key on the numeric keypad.
    KeypadMultiply,  ///< '*' key on the numeric keypad.
    KeypadAdd,       ///< '+' key on the numeric keypad.
    KeypadSeparator, ///< VK_SEPARATOR or DOM_VK_SEPARATOR
    KeypadSubtract,  ///< '-' key on the numeric keypad.
    KeypadDecimal,   ///< '.' key on the numeric keypad.
    KeypadDivide,    ///< '/' key on the numeric keypad.
    F1,              ///< F1 function key.
    F2,              ///< F2 function key.
    F3,              ///< F3 function key.
    F4,              ///< F4 function key.
    F5,              ///< F5 function key.
    F6,              ///< F6 function key.
    F7,              ///< F7 function key.
    F8,              ///< F8 function key.
    F9,              ///< F9 function key.
    F10,             ///< F10 function key.
    F11,             ///< F11 function key.
    F12,             ///< F12 function key.
    F13,             ///< F13 function key.
    F14,             ///< F14 function key.
    F15,             ///< F15 function key.
    F16,             ///< F16 function key.
    F17,             ///< F17 function key.
    F18,             ///< F18 function key.
    F19,             ///< F19 function key.
    F20,             ///< F20 function key.
    F21,             ///< F21 function key.
    F22,             ///< F22 function key.
    F23,             ///< F23 function key.
    F24,             ///< F24 function key.
    NumLock,         ///< NumLock key.
    ScrollLock,      ///< ScrollLock key.
    DoubleQuote,     ///< Double quote '"' key.
    Asterisk,        ///< Asterisk '*' key.
    Plus,            ///< Plus '+' key.
    Minus,           ///< Minus '-' key.
    Tilde,           ///< Tilde '~' key.
    VolumeMute,      ///< Volume Mute key.
    VolumeDown,      ///< Volume Down key.
    VolumeUp,        ///< Volume Up key.
    Comma,           ///< Comma ',' key.
    Period,          ///< Period '.' key.
    Slash,           ///< Slash '/' key.
    BackQuate,       ///< Back quate '`' key.
    OpenBracket,     ///< Open square bracket '[' key.
    Backslash,       ///< Back slash '\' key.
    CloseBracket,    ///< Close square bracket ']' key.
    Quote,           ///< Quate ''' key.
};

} // namespace pomdog
