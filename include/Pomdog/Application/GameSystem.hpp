//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_GAMESYSTEM_HPP
#define POMDOG_GAMESYSTEM_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"
#include "../Utility/Noncopyable.hpp"
#include <string>
#include <memory>
#include <functional>

namespace Pomdog {
namespace Details {

class SystemHub;

}// namespace Details

/// @addtogroup Framework
/// @{
/// @addtogroup Application
/// @{

///@~Japanese
/// @brief エンジンの各サブシステム間をつなぎます。
class GameSystem: private Noncopyable, public std::enable_shared_from_this<GameSystem>
{
public:
	GameSystem() = delete;
	GameSystem(GameSystem const&) = delete;
	GameSystem(GameSystem &&) = default;
	
	GameSystem & operator=(GameSystem const&) = delete;
	GameSystem & operator=(GameSystem &&) = default;

	explicit GameSystem(std::shared_ptr<Details::SystemHub> const& systemHub);
	
	virtual ~GameSystem();

	///@~Japanese
	/// @brief サブシステムを更新します。
	void Update();

	///@~Japanese
	/// @brief シーンのレンダリング前処理を行います。
	void RenderBegin();

	///@~Japanese
	/// @brief シーンをレンダリングします。
	void Render();

	///@~Japanese
	/// @brief シーンのレンダリング後の処理を行います。
	void RenderEnd();

	///@~Japanese 
	/// @brief 現在のヒープの状況を取得します。
	//void GetMemoryUsage(MemorySizer & sizer);

	///@~Japanese
	/// @brief 実行中のゲームおよびアプリケーションを終了します。
	void Quit();

	///@~Japanese
	/// @brief アプリケーションが終了処理を行っているかどうかを取得します。
	/// @return アプリケーションが終了処理に入っている場合は true を返します。
	/// @remarks GameSystem::Quit が正常に呼ばれた場合、終了処理に入ります。
	bool IsQuitting() const;

	///@~Japanese
	/// @brief 回復することが不可能なエラーが起きた場合、呼び出します。
	/// @param message エラーの詳細なメッセージを渡します。
	void FatalError(std::string const& message);

	///@~Japanese
	/// @brief ゲーム内でのバグが発見された場合に、呼び出します。
	/// @param message バグの詳細なメッセージを渡します。
	void ReportBug(std::string const& message);

	///@~Japanese
	/// @brief 深刻なエラーや未定義の動作を引き起こしかねない場合に、警告の通知を受け取ります。
	/// @param message 警告の詳細な内容です。
	void Warning(std::string const& message);
	
private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_GAMESYSTEM_HPP)
