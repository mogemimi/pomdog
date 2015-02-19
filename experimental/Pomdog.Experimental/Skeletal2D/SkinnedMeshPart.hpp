//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SKINNEDMESHPART_DAE847CA_8830_401A_8785_0EAB6DC603D5_HPP
#define POMDOG_SKINNEDMESHPART_DAE847CA_8830_401A_8785_0EAB6DC603D5_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

class SkinnedMeshPart {
public:
	///@brief 頂点の数
	std::uint16_t VertexCount;

	///@brief 頂点配列の先頭からのオフセット
	std::uint16_t VertexOffset;

	///@brief 描画されるプリミティブの数
	std::uint16_t PrimitiveCount;

	///@brief インデックス配列内の開始インデックス
	std::uint16_t StartIndex;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SKINNEDMESHPART_DAE847CA_8830_401A_8785_0EAB6DC603D5_HPP)
