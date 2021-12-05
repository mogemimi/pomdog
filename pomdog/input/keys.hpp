// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

/// Keys specifies keyboard keys and key codes.
enum class Keys : std::uint8_t {
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

    //
    // Alphanumerical keys
    //

    /// Zero key
    Alpha0 = 10,

    /// One key
    Alpha1 = 11,

    /// Two key
    Alpha2 = 12,

    /// Three key
    Alpha3 = 13,

    /// Four key
    Alpha4 = 14,

    /// Five key
    Alpha5 = 15,

    /// Six key
    Alpha6 = 16,

    /// Seven key
    Alpha7 = 17,

    /// Eight key
    Alpha8 = 18,

    /// Nine key
    Alpha9 = 19,

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

    //
    // Numeric keypad keys
    //

    /// Numeric keypad 0
    Keypad0 = 46,

    /// Numeric keypad 1
    Keypad1 = 47,

    /// Numeric keypad 2
    Keypad2 = 48,

    /// Numeric keypad 3
    Keypad3 = 49,

    /// Numeric keypad 4
    Keypad4 = 50,

    /// Numeric keypad 5
    Keypad5 = 51,

    /// Numeric keypad 6
    Keypad6 = 52,

    /// Numeric keypad 7
    Keypad7 = 53,

    /// Numeric keypad 8
    Keypad8 = 54,

    /// Numeric keypad 9
    Keypad9 = 55,

    /// Numeric keypad '.'
    KeypadDecimal = 56,

    /// Numeric keypad '+'
    KeypadAdd = 57,

    /// Numeric keypad '/'
    KeypadDivide = 58,

    /// Numeric keypad '*'
    KeypadMultiply = 59,

    /// Numeric keypad '-'
    KeypadSubtract = 60,

    //
    // keys on Arrow keypad
    //

    /// Up arrow key
    UpArrow = 61,

    /// Down arrow key
    DownArrow = 62,

    /// Left arrow key
    LeftArrow = 63,

    /// Right arrow key
    RightArrow = 64,

    /// Insert key
    Insert = 65,

    /// Home key
    Home = 66,

    /// End key
    End = 67,

    /// Page down key
    PageDown = 68,

    /// Page up key
    PageUp = 69,

    //
    // Function keys
    //

    /// F1 function key
    F1 = 71,

    /// F2 function key
    F2 = 72,

    /// F3 function key
    F3 = 73,

    /// F4 function key
    F4 = 74,

    /// F5 function key
    F5 = 75,

    /// F6 function key
    F6 = 76,

    /// F7 function key
    F7 = 77,

    /// F8 function key
    F8 = 78,

    /// F9 function key
    F9 = 79,

    /// F10 function key
    F10 = 80,

    /// F11 function key
    F11 = 81,

    /// F12 function key
    F12 = 82,

    /// F13 function key
    F13 = 83,

    /// F14 function key
    F14 = 84,

    /// F15 function key
    F15 = 85,

    //
    // Special keys
    //

    /// Clear key
    Clear = 90,

    /// Print key
    Print = 91,

    /// PrintScreen key
    PrintScreen = 92,

    /// NumLock key
    NumLock = 93,

    /// ScrollLock key
    ScrollLock = 94,

    /// Left Shift key
    LeftShift = 95,

    /// Right Shift key
    RightShift = 96,

    /// Left Control key
    LeftControl = 97,

    /// Right Control key
    RightControl = 98,

    /// Left Alt key
    LeftAlt = 99,

    /// Right Alt key
    RightAlt = 100,

    //
    // Windows and Apple keys
    //

    /// Left Apple key
    LeftApple = 101,

    /// Right Apple key
    RightApple = 102,

    /// Left Windows key
    LeftWindows = 103,

    /// Right Windows key
    RightWindows = 104,

    //
    // IME keys
    //

    /// IME Convert key
    IMEConvert = 105,

    /// IME NoConvert key
    IMENoConvert = 106,

    /// IME Kana mode key
    KanaMode = 107,

    /// IME Kanji mode key
    KanjiMode = 108,

    /// IME Hangul mode key
    HangulMode = 109,

    //
    // OEM keys
    //

    /// OEM ',' key
    Comma = 110,

    /// OEM '=' key
    Equals = 111,

    /// OEM ';' key
    Semicolon = 112,

    /// OEM '+' key
    Plus = 113,

    /// OEM '.' key
    Period = 114,

    /// OEM '/' key
    Slash = 115,

    /// OEM '-' key
    Minus = 116,

    /// OEM '?' key
    Question = 117,

    /// OEM ''' key
    Quote = 118,

    /// OEM '"' key
    DoubleQuote = 119,

    /// OEM '`' key
    AccentGrave = 120,

    /// OEM '\' key
    Backslash = 121,

    /// OEM '~' key
    Tilde = 122,

    /// OEM '[' key
    OpenBracket = 123,

    /// OEM ']' key
    CloseBracket = 124,
};

} // namespace Pomdog
