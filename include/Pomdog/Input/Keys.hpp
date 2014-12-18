//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_KEYS_13ECB5F8_A30A_4388_8785_2CDBD108AC25_HPP
#define POMDOG_KEYS_13ECB5F8_A30A_4388_8785_2CDBD108AC25_HPP

#if (_MSC_VER > 1000)
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
	BackSpace = 8,
	
	///@brief Tab key
	Tab = 9,
	
	///@brief Enter key
	Enter = 10,
	
	///@brief Escape key
	Escape = 11,
	
	///@brief Space key
	Space = 12,
	
	///@brief Delete key
	Delete = 13,
	
	///@brief Pause key
	Pause = 14,
	
	///@brief CapsLock key
	CapsLock = 15,
	
	///@brief Help key
	Help = 16,
	
	//
	// Alphanumerical keys
	//

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
	
	///@brief Zero key
	Alpha0 = 50,
	
	///@brief One key
	Alpha1 = 51,
	
	///@brief Two key
	Alpha2 = 52,
	
	///@brief Three key
	Alpha3 = 53,
	
	///@brief Four key
	Alpha4 = 54,
	
	///@brief Five key
	Alpha5 = 55,
	
	///@brief Six key
	Alpha6 = 56,
	
	///@brief Seven key
	Alpha7 = 57,
	
	///@brief Eight key
	Alpha8 = 58,
	
	///@brief Nine key
	Alpha9 = 59,

	//
	// Numeric keypad keys
	//

	///@brief Numeric keypad 0
	Keypad0 = 60,
	
	///@brief Numeric keypad 1
	Keypad1 = 61,
	
	///@brief Numeric keypad 2
	Keypad2 = 62,
	
	///@brief Numeric keypad 3
	Keypad3 = 63,
	
	///@brief Numeric keypad 4
	Keypad4 = 64,
	
	///@brief Numeric keypad 5
	Keypad5 = 65,
	
	///@brief Numeric keypad 6
	Keypad6 = 66,
	
	///@brief Numeric keypad 7
	Keypad7 = 67,
	
	///@brief Numeric keypad 8
	Keypad8 = 68,
	
	///@brief Numeric keypad 9
	Keypad9 = 69,
	
	///@brief Numeric keypad '.'
	KeypadDecimal = 70,
	
	///@brief Numeric keypad '+'
	KeypadAdd = 71,
	
	///@brief Numeric keypad '/'
	KeypadDivide = 72,
	
	///@brief Numeric keypad '*'
	KeypadMultiply = 73,
	
	///@brief Numeric keypad '-'
	KeypadSubtract = 74,

	//
	// keys on Arrow keypad
	//
	
	///@brief Up arrow key
	UpArrow = 80,
		
	///@brief Down arrow key
	DownArrow = 81,
	
	///@brief Left arrow key
	LeftArrow = 82,
	
	///@brief Right arrow key
	RightArrow = 83,
	
	///@brief Insert key
	Insert = 84,
	
	///@brief Home key
	Home = 85,
	
	///@brief End key
	End = 86,
	
	///@brief Page down key
	PageDown = 87,
	
	///@brief Page up key
	PageUp = 88,

	//
	// Function keys
	//
	
	///@brief F1 function key
	F1 = 101,
	
	///@brief F2 function key
	F2 = 102,
	
	///@brief F3 function key
	F3 = 103,
	
	///@brief F4 function key
	F4 = 104,
	
	///@brief F5 function key
	F5 = 105,
	
	///@brief F6 function key
	F6 = 106,
	
	///@brief F7 function key
	F7 = 107,
	
	///@brief F8 function key
	F8 = 108,
	
	///@brief F9 function key
	F9 = 109,
	
	///@brief F10 function key
	F10 = 110,
	
	///@brief F11 function key
	F11 = 111,
	
	///@brief F12 function key
	F12 = 112,
	
	///@brief F13 function key
	F13 = 113,
	
	///@brief F14 function key
	F14 = 114,
	
	///@brief F15 function key
	F15 = 115,

	//
	// Special keys
	//
	
	///@brief Clear key
	Clear = 130,
	
	///@brief Print key
	Print = 131,
	
	///@brief PrintScreen key
	PrintScreen = 132,

	///@brief NumLock key
	NumLock = 133,
	
	///@brief ScrollLock key
	ScrollLock = 134,
	
	///@brief Left Shift key
	LeftShift = 140,
	
	///@brief Right Shift key
	RightShift = 141,
	
	///@brief Left Control key
	LeftControl = 142,
	
	///@brief Right Control key
	RightControl = 143,
	
	///@brief Left Alt key
	LeftAlt = 144,
	
	///@brief Right Alt key
	RightAlt = 145,

	//
	// Windows and Apple keys
	//
	
	///@brief Left Apple key
	LeftApple = 150,
	
	///@brief Right Apple key
	RightApple = 151,
	
	///@brief Left Windows key
	LeftWindows = 152,
	
	///@brief Right Windows key
	RightWindows = 153,

	//
	// IME keys
	//
	
	///@brief IME Convert key
	IMEConvert = 160,
	
	///@brief IME NoConvert key
	IMENoConvert = 161,
	
	///@brief IME Kana mode key
	KanaMode = 162,
	
	///@brief IME Kanji mode key
	KanjiMode = 163,
	
	///@brief IME Hangul mode key
	HangulMode = 164,
	
	//
	// OEM keys
	//

	///@brief OEM ',' key
	Comma = 220,
	
	///@brief OEM '=' key
	Equals = 221,
	
	///@brief OEM ';' key
	Semicolon = 222,
	
	///@brief OEM '+' key
	Plus = 223,
	
	///@brief OEM '.' key
	Period = 224,
	
	///@brief OEM '/' key
	Slash = 225,
	
	///@brief OEM '-' key
	Minus = 226,
	
	///@brief OEM '?' key
	Question = 227,
	
	///@brief OEM ''' key
	Quote = 228,
	
	///@brief OEM '"' key
	DoubleQuote = 229,
	
	///@brief OEM '`' key
	AccentGrave = 230,

	///@brief OEM '\' key
	Backslash = 231,

	///@brief OEM '~' key
	Tilde = 232,
	
	///@brief OEM '[' key
	OpenBracket = 233,
	
	///@brief OEM ']' key
	CloseBracket = 234,
};

}// namespace Pomdog

#endif // !defined(POMDOG_KEYS_13ECB5F8_A30A_4388_8785_2CDBD108AC25_HPP)
