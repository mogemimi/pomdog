//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_NATIVEVERTEXBUFFER_37AE3D68_01A8_4D42_83CF_C2EE0D92AC91_HPP
#define POMDOG_SRC_RENDERSYSTEM_NATIVEVERTEXBUFFER_37AE3D68_01A8_4D42_83CF_C2EE0D92AC91_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Config/FundamentalTypes.hpp>
#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeVertexBuffer
{
public:
	NativeVertexBuffer() = default;
	NativeVertexBuffer(NativeVertexBuffer const&) = delete;
	NativeVertexBuffer & operator=(NativeVertexBuffer const&) = delete;

	virtual ~NativeVertexBuffer() = default;
	
	///@~Japanese
	/// @brief 頂点データを格納します。
	/// @remarks イミュータブルな頂点バッファでは使用しないでください。
	/// @param source ソースバッファを指定します。
	/// @param vertexCount 頂点の数を指定します。
	virtual void SetData(void const* source, std::uint32_t vertexCount,
		VertexDeclaration const& vertexDeclaration) = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVEVERTEXBUFFER_37AE3D68_01A8_4D42_83CF_C2EE0D92AC91_HPP)
