//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_VERTEXELEMENTUSAGE_F7AC2CCF_D5E4_40B5_85B5_365DFD78A900_HPP
#define POMDOG_VERTEXELEMENTUSAGE_F7AC2CCF_D5E4_40B5_85B5_365DFD78A900_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese 
/// @brief 頂点を構成する要素の使用方法を定義します。
enum class VertexElementUsage: std::uint8_t
{
	///@~Japanese
	/// @brief オブジェクト空間内での頂点位置を表します。
	Position,

	///@~Japanese
	/// @brief 頂点の法線ベクトルです。
	/// @remarks HLSL の頂点シェーダでは NORMAL[n] のセマンティクスで扱われます。
	Normal,

	///@~Japanese
	/// @brief 頂点のディフューズカラーまたはスペキュラカラーです。
	Color,

	///@~Japanese
	/// @brief 深度データです。
	Depth,

	///@~Japanese
	/// @brief 頂点の従法線ベクトルです。
	/// @remarks HLSL の頂点シェーダでは BINORMAL[n] のセマンティクスで扱われます。
	Binormal,

	///@~Japanese
	/// @brief 頂点の接線ベクトルです。
	/// @remarks HLSL の頂点シェーダでは TANGENT[n] のセマンティクスで扱われます。
	Tangent,
			
	///@~Japanese
	/// @brief テクスチャ座標です。
	TextureCoordinate,
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_VERTEXELEMENTUSAGE_F7AC2CCF_D5E4_40B5_85B5_365DFD78A900_HPP)
