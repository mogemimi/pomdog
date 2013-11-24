//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Application/GameSystem.hpp>
#include <Pomdog/Application/detail/SystemHub.hpp>

namespace Pomdog {

enum class GameSystemState: std::size_t
{
	///@~Japanese
	/// @brief 初期化前の状態を示します。
	NotInitialized = 0,

	///@~Japanese
	/// @brief 実行中であることを示します。
	Running = 1,

	///@~Japanese
	/// @brief 終了処理の最中であることを示します。
	Quitting = 2,
};

class GameSystem::Impl
{
public:
};

}// namespace Pomdog
