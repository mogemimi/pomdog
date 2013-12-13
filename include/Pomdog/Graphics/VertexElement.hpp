//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_VERTEXELEMENT_1A25E253_DA05_4C23_AEBD_4D04882E23E0_HPP
#define POMDOG_VERTEXELEMENT_1A25E253_DA05_4C23_AEBD_4D04882E23E0_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"
#include "VertexElementFormat.hpp"
#include "VertexElementUsage.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese 
/// @brief 頂点を構成する要素を定義します。
class VertexElement
{
public:
	///@~Japanese
	/// @brief バイト単位のオフセットを指定します。
	std::uint16_t Offset;

	///@~Japanese
	/// @brief 頂点を構成する要素のフォーマットを指定します。
	VertexElementFormat VertexElementFormat;

	///@~Japanese
	/// @brief 頂点を構成する要素の使用方法を指定します。
	VertexElementUsage VertexElementUsage;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_VERTEXELEMENT_1A25E253_DA05_4C23_AEBD_4D04882E23E0_HPP)
