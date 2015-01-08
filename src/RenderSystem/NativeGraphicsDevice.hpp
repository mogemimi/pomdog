//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_NATIVEGRAPHICSDEVICE_B06DA321_5506_4E99_8BB0_8704B3B97794_HPP
#define POMDOG_SRC_RENDERSYSTEM_NATIVEGRAPHICSDEVICE_B06DA321_5506_4E99_8BB0_8704B3B97794_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "../Utility/Noncopyable.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>
#include <vector>

namespace Pomdog {
namespace Details {

class ShaderBytecode;

namespace RenderSystem {

class NativeBlendState;
class NativeConstantBuffer;
class NativeDepthStencilState;
class NativeEffectPass;
class NativeEffectReflection;
class NativeIndexBuffer;
class NativeInputLayout;
class NativeRasterizerState;
class NativeRenderTarget2D;
class NativeSamplerState;
class NativeTexture2D;
class NativeVertexBuffer;

///@~Japanese
/// @brief グラフィックスリソースの作成を行います。
class NativeGraphicsDevice: Noncopyable {
public:
	virtual ~NativeGraphicsDevice() = default;

	///@~Japanese
	/// @brief 定数バッファを作成します。
	virtual std::unique_ptr<NativeConstantBuffer>
	CreateConstantBuffer(std::uint32_t sizeInBytes) = 0;

	///@~Japanese
	/// @brief インデックスバッファを作成します。
	virtual std::unique_ptr<NativeIndexBuffer>
	CreateIndexBuffer(std::uint32_t sizeInBytes, BufferUsage bufferUsage) = 0;

	///@~Japanese
	/// @brief インデックスバッファを作成します。
	virtual std::unique_ptr<NativeIndexBuffer>
	CreateIndexBuffer(void const* indices, std::uint32_t sizeInBytes,
		BufferUsage bufferUsage) = 0;
	
	///@~Japanese
	/// @brief 頂点バッファを作成します。
	virtual std::unique_ptr<NativeVertexBuffer>
	CreateVertexBuffer(std::uint32_t sizeInBytes, BufferUsage bufferUsage) = 0;
	
	///@~Japanese
	/// @brief 頂点バッファを作成します。
	virtual std::unique_ptr<NativeVertexBuffer>
	CreateVertexBuffer(void const* vertices, std::uint32_t sizeInBytes,
		BufferUsage bufferUsage) = 0;
	
	///@~Japanese
	/// @brief ブレンディングステートを作成します。
	virtual std::unique_ptr<NativeBlendState>
	CreateBlendState(BlendDescription const& description) = 0;

	///@~Japanese
	/// @brief 深度ステンシルステートを作成します。
	virtual std::unique_ptr<NativeDepthStencilState>
	CreateDepthStencilState(DepthStencilDescription const& description) = 0;

	///@~Japanese
	/// @brief サンプラーステートを作成します。
	virtual std::unique_ptr<NativeSamplerState>
	CreateSamplerState(SamplerDescription const& description) = 0;

	///@~Japanese
	/// @brief ラスタライザーステートを作成します。
	virtual std::unique_ptr<NativeRasterizerState>
	CreateRasterizerState(RasterizerDescription const& description) = 0;
		
	///@~Japanese
	/// @brief エフェクトパスを作成します。
	virtual std::unique_ptr<NativeEffectPass>
	CreateEffectPass(ShaderBytecode const& vertexShaderBytecode,
		ShaderBytecode const& pixelShaderBytecode) = 0;
	
	///@~Japanese
	/// @brief 指定されたエフェクトパスからエフェクトリフレクションを作成します。
	virtual std::unique_ptr<NativeEffectReflection>
	CreateEffectReflection(NativeEffectPass & nativeEffectPass) = 0;

	///@~Japanese
	/// @brief 入力レイアウトを作成します。
	virtual std::unique_ptr<NativeInputLayout>
	CreateInputLayout(NativeEffectPass & nativeEffectPass,
		std::vector<VertexBufferBinding> const& vertexBufferBindings) = 0;

	///@~Japanese
	/// @brief 2 次元テクスチャを作成します。
	virtual std::unique_ptr<NativeTexture2D>
	CreateTexture2D(std::int32_t width, std::int32_t height,
		std::uint32_t mipmapLevels, SurfaceFormat format) = 0;
	
	///@~Japanese
	/// @brief 2 次元のレンダーターゲットを作成します。
	virtual std::unique_ptr<NativeRenderTarget2D>
	CreateRenderTarget2D(std::int32_t width, std::int32_t height,
		std::uint32_t mipmapLevels, SurfaceFormat format, DepthFormat depthStencilFormat) = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVEGRAPHICSDEVICE_B06DA321_5506_4E99_8BB0_8704B3B97794_HPP)
