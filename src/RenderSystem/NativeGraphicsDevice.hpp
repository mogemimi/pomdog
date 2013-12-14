//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_NATIVEGRAPHICSDEVICE_B06DA321_5506_4E99_8BB0_8704B3B97794_HPP
#define POMDOG_SRC_RENDERSYSTEM_NATIVEGRAPHICSDEVICE_B06DA321_5506_4E99_8BB0_8704B3B97794_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include <Pomdog/Utility/Noncopyable.hpp>
#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeBlendState;
class NativeDepthStencilState;
class NativeIndexBuffer;
class NativeSamplerState;
class NativeRasterizerState;
class NativeVertexBuffer;

///@~Japanese
/// @brief グラフィックスリソースの作成を行います。
class NativeGraphicsDevice: Noncopyable
{
public:
	virtual ~NativeGraphicsDevice() = default;

	///@~Japanese
	/// @brief インデックスバッファを作成します。
	virtual std::unique_ptr<NativeIndexBuffer> CreateIndexBuffer(
		void const* indices, std::size_t indexCount,
		IndexElementSize elementSize, BufferUsage bufferUsage) = 0;
	
	///@~Japanese
	/// @brief 頂点バッファを作成します。
	virtual std::unique_ptr<NativeVertexBuffer> CreateVertexBuffer(
		void const* vertices, std::size_t vertexCount,
		VertexDeclaration const& vertexDeclaration, BufferUsage bufferUsage) = 0;
	
	///@~Japanese
	/// @brief ブレンディングステートを作成します。
	virtual std::unique_ptr<NativeBlendState> CreateBlendState(
		BlendDescription const& description) = 0;

	///@~Japanese
	/// @brief 深度ステンシルステートを作成します。
	virtual std::unique_ptr<NativeDepthStencilState> CreateDepthStencilState(
		DepthStencilDescription const& description) = 0;

	///@~Japanese
	/// @brief サンプラーステートを作成します。
	virtual std::unique_ptr<NativeSamplerState> CreateSamplerState(
		SamplerDescription const& description) = 0;

	///@~Japanese
	/// @brief ラスタライザーステートを作成します。
	virtual std::unique_ptr<NativeRasterizerState> CreateRasterizerState(
		RasterizerDescription const& description) = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVEGRAPHICSDEVICE_B06DA321_5506_4E99_8BB0_8704B3B97794_HPP)
