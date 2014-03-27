//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DEPTHSTENCILDESCRIPTION_E14C5D77_3D8D_4124_95C0_4B8DBDBE2D63_HPP
#define POMDOG_DEPTHSTENCILDESCRIPTION_E14C5D77_3D8D_4124_95C0_4B8DBDBE2D63_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>
#include "../Config/Export.hpp"
#include "ComparisonFunction.hpp"
#include "DepthStencilOperation.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief 深度ステンシルテストを記述します。
class POMDOG_EXPORT DepthStencilDescription {
public:
	///@~Japanese
	/// @brief ワインディング順序が時計回りの三角形の深度ステンシルテストの結果の使用方法を指定します。
	DepthStencilOperation ClockwiseFace;

	///@~Japanese
	/// @brief ワインディング順序が反時計回りの三角形の深度ステンシルテストの結果の使用方法を指定します。
	DepthStencilOperation CounterClockwiseFace;

	///@~Japanese
	/// @brief ステンシルテストの比較に使用する基準値を指定します。デフォルト値は 0 です。
	std::int32_t ReferenceStencil;

	///@~Japanese
	/// @brief ステンシルバッファの中で、ステンシルデータの読み取り部分を識別するために適用されるマスクです。
	/// デフォルト値は std::numeric_limits<std::uint32_t>::max() です。
	/// @remarks デフォルト値は、全てのビットが 1 になっている符号なし整数の最大値です。
	std::uint32_t StencilMask;

	///@~Japanese
	/// @brief ステンシルバッファの中で、ステンシルデータの書き込み部分を識別するために適用されるマスクです。
	/// デフォルト値は std::numeric_limits<std::uint32_t>::max() です。
	/// @remarks デフォルト値は、全てのビットが 1 になっている符号なし整数の最大値です。
	std::uint32_t StencilWriteMask;

	///@~Japanese
	/// @brief 深度テストで使用する比較関数を指定します。デフォルト値は ComparisonFunction::LessEqual です。
	ComparisonFunction DepthBufferFunction;

	///@~Japanese
	/// @brief 深度テストを有効または無効にします。デフォルト値は有効 (true) です。
	/// @remarks 深度テストを有効にする場合は true を、無効にする場合は false を指定します。
	bool DepthBufferEnable;

	///@~Japanese
	/// @brief 深度バッファへの書き込みを有効または無効にします。デフォルト値は有効 (true) です。
	/// @remarks 深度バッファへの書き込みを有効にする場合は true を、
	/// 無効にする場合は false を指定します。
	bool DepthBufferWriteEnable;

	///@~Japanese
	/// @brief ステンシルテストを有効または無効にします。デフォルト値は無効 (false) です。
	bool StencilEnable;

public:
	DepthStencilDescription();
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_DEPTHSTENCILDESCRIPTION_E14C5D77_3D8D_4124_95C0_4B8DBDBE2D63_HPP)
