//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

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

///@~Japanese
/// @brief ゲームウィンドウです。
class POMDOG_EXPORT GameWindow {
public:
	GameWindow() = default;
	GameWindow(GameWindow const&) = delete;
	GameWindow & operator=(GameWindow const&) = delete;

	virtual ~GameWindow() = default;

	///@~Japanese
	/// @brief プレイヤーによるウィンドウサイズの変更を許可するかどうかを取得します。
	/// @return プレイヤーによるウィンドウサイズの変更を許可する場合は true を返却します。
	/// @note Windows Store アプリは、この値に関わらず
	/// プレイヤーによるウィンドウサイズの変更が許可されています。
	virtual bool AllowPlayerResizing() const = 0;

	///@~Japanese
	/// @brief プレイヤーによるウィンドウサイズの変更を許可するかどうかを設定します。
	/// @param allowResizing プレイヤーによるウィンドウサイズの変更を許可する場合は true を指定します。
	virtual void AllowPlayerResizing(bool allowResizing) = 0;

	///@~Japanese
	/// @brief 現在のウィンドウタイトルをを文字列として取得します。
	virtual std::string Title() const = 0;

	///@~Japanese
	/// @brief ウィンドウタイトルを設定します。
	virtual void Title(std::string const& title) = 0;

	///@~Japanese
	/// @brief 現在のクライアント領域のサイズを取得します。
	/// @return クライアント領域を表すサイズを返却します。
	virtual Rectangle ClientBounds() const = 0;

	///@~Japanese
	/// @brief ゲームウィンドウのクライアント領域のサイズを設定します。
	/// @param clientBounds ゲームウィンドウのクライアント領域のサイズを指定します。
	virtual void ClientBounds(Rectangle const& clientBounds) = 0;

	///@brief Return true if the mouse cursor is visible, false otherwise.
	virtual bool IsMouseCursorVisible() const = 0;

	virtual void IsMouseCursorVisible(bool visible) = 0;

	virtual void SetMouseCursor(MouseCursor cursor) = 0;

	///@~Japanese
	/// @brief ゲームウィンドウのクライアント領域のサイズが変更されたときに呼ばれるイベントです。
	Signal<void(int width, int height)> ClientSizeChanged;
};

}// namespace Pomdog

#endif // !defined(POMDOG_GAMEWINDOW_22D747BC_F865_4F1E_B8BB_609D297F662F_HPP)
