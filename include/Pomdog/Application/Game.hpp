//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_GAME_8B7C90B7_0C6E_4161_9C57_2BDE1DC6DD57_HPP
#define POMDOG_GAME_8B7C90B7_0C6E_4161_9C57_2BDE1DC6DD57_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "Pomdog/Config/Export.hpp"

namespace Pomdog {

///@~Japanese
/// @brief ゲームの内容を記述します。
/// @remarks プラットフォームを意識することなくゲームのコンテンツ開発に集中できる
/// ようにします。
class POMDOG_EXPORT Game {
public:
	Game() = default;
	Game(Game const&) = delete;
	Game & operator=(Game const&) = delete;

	virtual ~Game() = default;

	///@~Japanese
	/// @brief アプリケーションの初期化を行います。
	virtual void Initialize() = 0;

	///@~Japanese
	/// @brief ゲームの初期化が完了しているかどうかを取得します。
	/// @return 初期化が完了している場合は true を、未初期化の場合は false を返します。
	virtual bool CompleteInitialize() const { return true; }

	///@~Japanese
	/// @brief ゲームの更新を行います。
	virtual void Update() = 0;

	///@~Japanese
	/// @brief シーンがレンダリングされる際に毎フレーム呼び出されます。
	virtual void Draw() = 0;
};

}// namespace Pomdog

#endif // !defined(POMDOG_GAME_8B7C90B7_0C6E_4161_9C57_2BDE1DC6DD57_HPP)
