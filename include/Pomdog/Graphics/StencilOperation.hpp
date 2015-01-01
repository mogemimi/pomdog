//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_STENCILOPERATION_6EB1EB6D_6B2D_4B7B_BA58_1AEA11222D30_HPP
#define POMDOG_STENCILOPERATION_6EB1EB6D_6B2D_4B7B_BA58_1AEA11222D30_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

///@~Japanese
/// @brief ステンシルテスト時のステンシルバッファの処理を定義します。
enum class StencilOperation: std::uint8_t {
	///@~Japanese
	/// @brief 既存のステンシルデータを保ちます。
	/// @remarks デフォルト値です。
	Keep,

	///@~Japanese
	/// @brief ステンシルデータの値を 0 に設定します。
	Zero,

	///@~Japanese
	/// @brief ステンシルデータを設定された参照値で置き換えます。
	Replace,

	///@~Japanese
	/// @brief ステンシルの値をインクリメントします。結果はクランプされます。
	IncrementSaturation,

	///@~Japanese
	/// @brief ステンシルの値をデクリメントします。結果はクランプされます。
	DecrementSaturation,

	///@~Japanese
	/// @brief ステンシルデータのビットを反転します。
	Invert,

	///@~Japanese
	/// @brief ステンシルの値をインクリメントします。必要に応じて結果はラップされます。
	/// @remarks インクリメントによってその結果が最大値を超えると、ステンシルの値はラップされ 0 に戻ります。
	Increment,

	///@~Japanese
	/// @brief ステンシルの値をデクリメントします。必要に応じて結果はラップされます。
	/// @remarks デクリメントによってその結果が 0 より小さくなると、ステンシルの値はラップされ最大値に戻ります。
	Decrement,
};

}// namespace Pomdog

#endif // !defined(POMDOG_STENCILOPERATION_6EB1EB6D_6B2D_4B7B_BA58_1AEA11222D30_HPP)
