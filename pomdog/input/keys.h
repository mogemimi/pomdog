// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog {

/// Keys specifies keyboard keys and key codes.
enum class Keys : u8 {
    /// Reserved (never is pressed)
    Unknown = 0,

    /// BackSpace key
    BackSpace = 1,

    /// Tab key
    Tab = 2,

    /// Enter key
    Enter = 3,

    /// Escape key
    Escape = 4,

    /// Space key
    Space = 5,

    /// Delete key
    Delete = 6,

    /// Pause key
    Pause = 7,

    /// CapsLock key
    CapsLock = 8,

    /// Help key
    Help = 9,

    /// "0" key
    Digit0 = 10,

    /// "1" key
    Digit1 = 11,

    /// "2" key
    Digit2 = 12,

    /// "3" key
    Digit3 = 13,

    /// "4" key
    Digit4 = 14,

    /// "5" key
    Digit5 = 15,

    /// "6" key
    Digit6 = 16,

    /// "7" key
    Digit7 = 17,

    /// "8" key
    Digit8 = 18,

    /// "9" key
    Digit9 = 19,

    /// Letter A key
    A = 20,

    /// Letter B key
    B = 21,

    /// Letter C key
    C = 22,

    /// Letter D key
    D = 23,

    /// Letter E key
    E = 24,

    /// Letter F key
    F = 25,

    /// Letter G key
    G = 26,

    /// Letter H key
    H = 27,

    /// Letter I key
    I = 28,

    /// Letter J key
    J = 29,

    /// Letter K key
    K = 30,

    /// Letter L key
    L = 31,

    /// Letter M key
    M = 32,

    /// Letter N key
    N = 33,

    /// Letter O key
    O = 34,

    /// Letter P key
    P = 35,

    /// Letter Q key
    Q = 36,

    /// Letter R key
    R = 37,

    /// Letter S key
    S = 38,

    /// Letter T key
    T = 39,

    /// Letter U key
    U = 40,

    /// Letter V key
    V = 41,

    /// Letter W key
    W = 42,

    /// Letter X key
    X = 43,

    /// Letter Y key
    Y = 44,

    /// Letter Z key
    Z = 45,

    /// Sleep key
    Sleep = 46,

    /// Numeric keypad 0 key
    Numpad0 = 47,

    /// Numeric keypad 1 key
    Numpad1 = 48,

    /// Numeric keypad 2 key
    Numpad2 = 49,

    /// Numeric keypad 3 key
    Numpad3 = 50,

    /// Numeric keypad 4 key
    Numpad4 = 51,

    /// Numeric keypad 5 key
    Numpad5 = 52,

    /// Numeric keypad 6 key
    Numpad6 = 53,

    /// Numeric keypad 7 key
    Numpad7 = 54,

    /// Numeric keypad 8 key
    Numpad8 = 55,

    /// Numeric keypad 9 key
    Numpad9 = 56,

    /// Numeric keypad "." and "Del" key
    NumpadDecimal = 57,

    /// Numeric keypad "+" key
    NumpadAdd = 58,

    /// Numeric keypad "/" key
    NumpadDivide = 59,

    /// Numeric keypad "*" key
    NumpadMultiply = 60,

    /// Numeric keypad "-" key
    NumpadSubtract = 61,

    /// Numeric keypad separator key
    NumpadSeparator = 62,

    /// F0 function key
    F0 = 63,

    /// F1 function key
    F1 = 64,

    /// F2 function key
    F2 = 65,

    /// F3 function key
    F3 = 66,

    /// F4 function key
    F4 = 67,

    /// F5 function key
    F5 = 68,

    /// F6 function key
    F6 = 69,

    /// F7 function key
    F7 = 70,

    /// F8 function key
    F8 = 71,

    /// F9 function key
    F9 = 72,

    /// F10 function key
    F10 = 73,

    /// F11 function key
    F11 = 74,

    /// F12 function key
    F12 = 75,

    /// F13 function key
    F13 = 76,

    /// F14 function key
    F14 = 77,

    /// F15 function key
    F15 = 78,

    /// F16 function key
    F16 = 79,

    /// F17 function key
    F17 = 80,

    /// F18 function key
    F18 = 81,

    /// F19 function key
    F19 = 82,

    /// F20 function key
    F20 = 83,

    /// F21 function key
    F21 = 84,

    /// F22 function key
    F22 = 85,

    /// F23 function key
    F23 = 86,

    /// F24 function key
    F24 = 87,

    /// Up arrow key
    UpArrow = 88,

    /// Down arrow key
    DownArrow = 89,

    /// Left arrow key
    LeftArrow = 90,

    /// Right arrow key
    RightArrow = 91,

    /// Insert key
    Insert = 92,

    /// Home key
    Home = 93,

    /// End key
    End = 94,

    /// Page down key
    PageDown = 95,

    /// Page up key
    PageUp = 96,

    /// Execute key
    Execute = 97,

    /// Select key
    Select = 98,

    /// Clear key
    Clear = 99,

    /// Print key
    Print = 100,

    /// PrintScreen key
    PrintScreen = 101,

    /// NumLock key
    NumLock = 102,

    /// ScrollLock key
    ScrollLock = 103,

    /// Left Shift key
    LeftShift = 104,

    /// Right Shift key
    RightShift = 105,

    /// Left Control key
    LeftControl = 106,

    /// Right Control key
    RightControl = 107,

    /// Left Alt key
    LeftAlt = 108,

    /// Right Alt key
    RightAlt = 109,

    /// Left Apple/Windows key
    LeftSuper = 110,

    /// Right Apple/Windows key
    RightSuper = 111,

    /// Application Context Menu key
    ContextMenu = 112,

    /// IME Convert (henkan) key on Japanese keyboard
    Convert = 113,

    /// IME NonConvert (muhenkan) key on Japanese keyboard
    NonConvert = 114,

    /// "Kana" (katakana/hiragana/romaji) key on Japanese keyboard
    KanaMode = 115,

    /// "Kanji" key on Japanese keyboard, "Hanja" key on Korean keyboard
    KanjiMode = 116,

    /// "Hangul" key on Korean keyboard
    HangulMode = 117,

    /// "Junja" key on Korean keyboard
    JunjaMode = 118,

    /// Labeled "\|" key on US keyboard
    IntlBackslash = 119,

    /// Labeled "Ro" key on Japanese keyboard
    IntlRo = 120,

    /// Labeled "Yen" key on Japanese keyboard
    IntlYen = 121,

    /// OEM "," key
    Comma = 122,

    /// OEM "=" key
    Equals = 123,

    /// OEM ";" key
    Semicolon = 124,

    /// OEM "+" key
    Plus = 125,

    /// OEM "." key
    Period = 126,

    /// OEM "/" key
    Slash = 127,

    /// OEM "-" key
    Minus = 128,

    /// OEM "?" key
    Question = 129,

    /// OEM "'" key
    Quote = 130,

    /// OEM '"' key
    DoubleQuote = 131,

    /// OEM "`" key
    Backquote = 132,

    /// OEM "\" key
    Backslash = 133,

    /// OEM "~" key
    Tilde = 134,

    /// OEM "[" key
    OpenBracket = 135,

    /// OEM "]" key
    CloseBracket = 136,

    /// Volume Down key
    AudioVolumeDown = 137,

    /// Volume Up key
    AudioVolumeUp = 138,

    /// Audio Mute key
    AudioVolumeMute = 139,
};

/// Defines the maximum number of keys, which is used for bitset size.
inline constexpr u32 MaxKeysCount = static_cast<u32>(Keys::AudioVolumeMute) + 1;

} // namespace pomdog
