//
//  Copyright (C) 2013-2014 mogemimi.
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

#include "../Config/Export.hpp"
#include "../Config/FundamentalTypes.hpp"
#include "VertexElementFormat.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese 
/// @brief 頂点を構成する要素を定義します。
class POMDOG_EXPORT VertexElement {
public:
	///@~Japanese
	/// @brief 頂点データの先頭から頂点要素までのバイト単位のオフセットを指定します。
	std::uint16_t Offset;

	///@~Japanese
	/// @brief 頂点を構成する要素のフォーマットを指定します。
	VertexElementFormat VertexElementFormat;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_VERTEXELEMENT_1A25E253_DA05_4C23_AEBD_4D04882E23E0_HPP)
