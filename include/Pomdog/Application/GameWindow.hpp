// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GAMEWINDOW_22D747BC_F865_4F1E_B8BB_609D297F662F_HPP
#define POMDOG_GAMEWINDOW_22D747BC_F865_4F1E_B8BB_609D297F662F_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Event/Signal.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <string>
#include <cstdint>

namespace Pomdog {

class Rectangle;
enum class MouseCursor: std::uint8_t;

class POMDOG_EXPORT GameWindow {
public:
	GameWindow() = default;
	GameWindow(GameWindow const&) = delete;
	GameWindow & operator=(GameWindow const&) = delete;

	virtual ~GameWindow() = default;

	virtual bool AllowPlayerResizing() const = 0;

	virtual void AllowPlayerResizing(bool allowResizing) = 0;

	virtual std::string Title() const = 0;

	virtual void Title(std::string const& title) = 0;

	virtual Rectangle ClientBounds() const = 0;

	virtual void ClientBounds(Rectangle const& clientBounds) = 0;

	///@brief Return true if the mouse cursor is visible, false otherwise.
	virtual bool IsMouseCursorVisible() const = 0;

	virtual void IsMouseCursorVisible(bool visible) = 0;

	virtual void SetMouseCursor(MouseCursor cursor) = 0;

	Signal<void(int width, int height)> ClientSizeChanged;
};

}// namespace Pomdog

#endif // !defined(POMDOG_GAMEWINDOW_22D747BC_F865_4F1E_B8BB_609D297F662F_HPP)
