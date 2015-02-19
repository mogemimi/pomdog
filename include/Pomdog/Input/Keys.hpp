//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_KEYS_13ECB5F8_A30A_4388_8785_2CDBD108AC25_HPP
#define POMDOG_KEYS_13ECB5F8_A30A_4388_8785_2CDBD108AC25_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

///@~English
/// @brief Specifies keyboard keys and key codes.
///@~Japanese
/// @brief キーボードのキーとキーコードを定義します。
enum class Keys: std::uint8_t {
	///@brief Reserved (never is pressed)
	None = 0,

	///@brief BackSpace key
	BackSpace = 1,

	///@brief Tab key
	Tab = 2,

	///@brief Enter key
	Enter = 3,

	///@brief Escape key
	Escape = 4,

	///@brief Space key
	Space = 5,

	///@brief Delete key
	Delete = 6,

	///@brief Pause key
	Pause = 7,

	///@brief CapsLock key
	CapsLock = 8,

	///@brief Help key
	Help = 9,

	//
	// Alphanumerical keys
	//

	///@brief Zero key
	Alpha0 = 10,

	///@brief One key
	Alpha1 = 11,

	///@brief Two key
	Alpha2 = 12,

	///@brief Three key
	Alpha3 = 13,

	///@brief Four key
	Alpha4 = 14,

	///@brief Five key
	Alpha5 = 15,

	///@brief Six key
	Alpha6 = 16,

	///@brief Seven key
	Alpha7 = 17,

	///@brief Eight key
	Alpha8 = 18,

	///@brief Nine key
	Alpha9 = 19,

	///@brief Letter A key
	A = 20,

	///@brief Letter B key
	B = 21,

	///@brief Letter C key
	C = 22,

	///@brief Letter D key
	D = 23,

	///@brief Letter E key
	E = 24,

	///@brief Letter F key
	F = 25,

	///@brief Letter G key
	G = 26,

	///@brief Letter H key
	H = 27,

	///@brief Letter I key
	I = 28,

	///@brief Letter J key
	J = 29,

	///@brief Letter K key
	K = 30,

	///@brief Letter L key
	L = 31,

	///@brief Letter M key
	M = 32,

	///@brief Letter N key
	N = 33,

	///@brief Letter O key
	O = 34,

	///@brief Letter P key
	P = 35,

	///@brief Letter Q key
	Q = 36,

	///@brief Letter R key
	R = 37,

	///@brief Letter S key
	S = 38,

	///@brief Letter T key
	T = 39,

	///@brief Letter U key
	U = 40,

	///@brief Letter V key
	V = 41,

	///@brief Letter W key
	W = 42,

	///@brief Letter X key
	X = 43,

	///@brief Letter Y key
	Y = 44,

	///@brief Letter Z key
	Z = 45,

	//
	// Numeric keypad keys
	//

	///@brief Numeric keypad 0
	Keypad0 = 46,

	///@brief Numeric keypad 1
	Keypad1 = 47,

	///@brief Numeric keypad 2
	Keypad2 = 48,

	///@brief Numeric keypad 3
	Keypad3 = 49,

	///@brief Numeric keypad 4
	Keypad4 = 50,

	///@brief Numeric keypad 5
	Keypad5 = 51,

	///@brief Numeric keypad 6
	Keypad6 = 52,

	///@brief Numeric keypad 7
	Keypad7 = 53,

	///@brief Numeric keypad 8
	Keypad8 = 54,

	///@brief Numeric keypad 9
	Keypad9 = 55,

	///@brief Numeric keypad '.'
	KeypadDecimal = 56,

	///@brief Numeric keypad '+'
	KeypadAdd = 57,

	///@brief Numeric keypad '/'
	KeypadDivide = 58,

	///@brief Numeric keypad '*'
	KeypadMultiply = 59,

	///@brief Numeric keypad '-'
	KeypadSubtract = 60,

	//
	// keys on Arrow keypad
	//

	///@brief Up arrow key
	UpArrow = 61,

	///@brief Down arrow key
	DownArrow = 62,

	///@brief Left arrow key
	LeftArrow = 63,

	///@brief Right arrow key
	RightArrow = 64,

	///@brief Insert key
	Insert = 65,

	///@brief Home key
	Home = 66,

	///@brief End key
	End = 67,

	///@brief Page down key
	PageDown = 68,

	///@brief Page up key
	PageUp = 69,

	//
	// Function keys
	//

	///@brief F1 function key
	F1 = 71,

	///@brief F2 function key
	F2 = 72,

	///@brief F3 function key
	F3 = 73,

	///@brief F4 function key
	F4 = 74,

	///@brief F5 function key
	F5 = 75,

	///@brief F6 function key
	F6 = 76,

	///@brief F7 function key
	F7 = 77,

	///@brief F8 function key
	F8 = 78,

	///@brief F9 function key
	F9 = 79,

	///@brief F10 function key
	F10 = 80,

	///@brief F11 function key
	F11 = 81,

	///@brief F12 function key
	F12 = 82,

	///@brief F13 function key
	F13 = 83,

	///@brief F14 function key
	F14 = 84,

	///@brief F15 function key
	F15 = 85,

	//
	// Special keys
	//

	///@brief Clear key
	Clear = 90,

	///@brief Print key
	Print = 91,

	///@brief PrintScreen key
	PrintScreen = 92,

	///@brief NumLock key
	NumLock = 93,

	///@brief ScrollLock key
	ScrollLock = 94,

	///@brief Left Shift key
	LeftShift = 95,

	///@brief Right Shift key
	RightShift = 96,

	///@brief Left Control key
	LeftControl = 97,

	///@brief Right Control key
	RightControl = 98,

	///@brief Left Alt key
	LeftAlt = 99,

	///@brief Right Alt key
	RightAlt = 100,

	//
	// Windows and Apple keys
	//

	///@brief Left Apple key
	LeftApple = 101,

	///@brief Right Apple key
	RightApple = 102,

	///@brief Left Windows key
	LeftWindows = 103,

	///@brief Right Windows key
	RightWindows = 104,

	//
	// IME keys
	//

	///@brief IME Convert key
	IMEConvert = 105,

	///@brief IME NoConvert key
	IMENoConvert = 106,

	///@brief IME Kana mode key
	KanaMode = 107,

	///@brief IME Kanji mode key
	KanjiMode = 108,

	///@brief IME Hangul mode key
	HangulMode = 109,

	//
	// OEM keys
	//

	///@brief OEM ',' key
	Comma = 110,

	///@brief OEM '=' key
	Equals = 111,

	///@brief OEM ';' key
	Semicolon = 112,

	///@brief OEM '+' key
	Plus = 113,

	///@brief OEM '.' key
	Period = 114,

	///@brief OEM '/' key
	Slash = 115,

	///@brief OEM '-' key
	Minus = 116,

	///@brief OEM '?' key
	Question = 117,

	///@brief OEM ''' key
	Quote = 118,

	///@brief OEM '"' key
	DoubleQuote = 119,

	///@brief OEM '`' key
	AccentGrave = 120,

	///@brief OEM '\' key
	Backslash = 121,

	///@brief OEM '~' key
	Tilde = 122,

	///@brief OEM '[' key
	OpenBracket = 123,

	///@brief OEM ']' key
	CloseBracket = 124,
};

}// namespace Pomdog

#endif // !defined(POMDOG_KEYS_13ECB5F8_A30A_4388_8785_2CDBD108AC25_HPP)
