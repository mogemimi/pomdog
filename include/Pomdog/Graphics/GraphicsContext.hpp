//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_GRAPHICSCONTEXT_F873DD34_5F2E_44DD_A419_0E78FE2D56E5_HPP
#define POMDOG_GRAPHICSCONTEXT_F873DD34_5F2E_44DD_A419_0E78FE2D56E5_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include "Pomdog/Config/Export.hpp"
#include <cstdint>
#include <cstddef>
#include <memory>
#include <vector>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeGraphicsContext;
class PresentationParameters;

}// namespace RenderSystem
}// namespace Details

///@~Japanese
/// @brief 下位レベルの API を提供するグラフィックスコンテキストです。レンダリングコマンドを生成するために使用します。
class POMDOG_EXPORT GraphicsContext {
public:
	GraphicsContext() = delete;
	GraphicsContext(GraphicsContext const&) = delete;
	GraphicsContext & operator=(GraphicsContext const&) = delete;

	GraphicsContext(std::unique_ptr<Details::RenderSystem::NativeGraphicsContext> nativeContext,
		Details::RenderSystem::PresentationParameters const& presentationParameters,
		std::shared_ptr<GraphicsDevice> const& graphicsDevice);
	
	~GraphicsContext();
	
	///@~Japanese
	/// @brief 現在バインドされているレンダーターゲットを指定された色でクリアーします。
	void Clear(Color const& color);
	
	///@~Japanese
	/// @brief 指定されたレンダーターゲットまたは深度ステンシルバッファをクリアーします。
	void Clear(ClearOptions options, Color const& color, float depth, std::int32_t stencil);

	///@~Japanese
	/// @brief バックバッファの内容をディスプレイに表示します。
	void Present();
	
	///@~Japanese
	/// @brief インデックスを持たず、インスタンス化されていないプリミティブを描画します。
	/// @param primitiveTopology プリミティブの位相。
	/// @param vertexCount レンダリングする頂点の数。
	void Draw(PrimitiveTopology primitiveTopology, std::uint32_t vertexCount);

	///@~Japanese
	/// @brief インデックス付けされた頂点データに基づき、インスタンス化されていないプリミティブを描画します。
	/// @param primitiveTopology プリミティブの位相。
	/// @param indexBuffer インデックスバッファ。
	/// @param indexCount インデックスの要素数。
	void DrawIndexed(PrimitiveTopology primitiveTopology,
		std::shared_ptr<IndexBuffer> const& indexBuffer, std::uint32_t indexCount);

	///@~Japanese
	/// @brief インデックスを持たない、インスタンス化されたプリミティブを描画します。
	/// @param primitiveTopology プリミティブの位相。
	/// @param vertexCount レンダリングする頂点の数。
	/// @param instanceCount レンダリングするインスタンスの数。
	void DrawInstanced(PrimitiveTopology primitiveTopology,
		std::uint32_t vertexCount, std::uint32_t instanceCount);

	///@~Japanese
	/// @brief インデックス付けされた頂点データに基づき、インスタンス化されたプリミティブを描画します。
	/// @param primitiveTopology プリミティブの位相。
	/// @param indexBuffer インデックスバッファ。
	/// @param indexCount インデックスの要素数。
	/// @param instanceCount レンダリングするインスタンスの数。
	void DrawIndexedInstanced(PrimitiveTopology primitiveTopology,
		std::shared_ptr<IndexBuffer> const& indexBuffer, std::uint32_t indexCount, std::uint32_t instanceCount);
	
	///@~Japanese
	/// @brief ビューポートを取得します。
	Pomdog::Viewport const& Viewport() const;

	///@~Japanese
	/// @brief ビューポートを設定します。
	void Viewport(Pomdog::Viewport const& viewport);
	
	///@~Japanese
	/// @brief シザーテストで用いるシザー矩形を取得します。
	Pomdog::Rectangle ScissorRectangle() const;
	
	///@~Japanese
	/// @brief シザーテストで用いるシザー矩形を設定します。
	void ScissorRectangle(Pomdog::Rectangle const& rectangle);
	
	///@~Japanese
	/// @brief ブレンディングステートを取得します。
	std::shared_ptr<BlendState> GetBlendState() const;
	
	///@~Japanese
	/// @brief ブレンディングステートを設定します。
	void SetBlendState(std::shared_ptr<BlendState> const& blendState);

	///@~Japanese
	/// @brief 深度ステンシルステートを取得します。
	std::shared_ptr<DepthStencilState> GetDepthStencilState() const;

	///@~Japanese
	/// @brief 深度ステンシルステートを設定します。
	void SetDepthStencilState(std::shared_ptr<DepthStencilState> const& depthStencilState);

	///@~Japanese
	/// @brief ラスタライザーステートを取得します。
	std::shared_ptr<RasterizerState> GetRasterizerState() const;
	
	///@~Japanese
	/// @brief ラスタライザーステートを設定します。
	void SetRasterizerState(std::shared_ptr<RasterizerState> const& rasterizerState);

	///@~Japanese
	/// @brief テクスチャサンプラーステートを設定します。
	void SetSamplerState(std::uint32_t index, std::shared_ptr<SamplerState> const& samplerState);

	///@~Japanese
	/// @brief 入力レイアウトを設定します。
	void SetInputLayout(std::shared_ptr<InputLayout> const& inputLayout);

	///@~Japanese
	/// @brief 頂点バッファを設定します。
	void SetVertexBuffer(std::shared_ptr<VertexBuffer> const& vertexBuffer);
	
	///@~Japanese
	/// @brief 頂点バッファの配列を設定します。
	void SetVertexBuffers(std::vector<std::shared_ptr<VertexBuffer>> const& vertexBuffers);

	///@~Japanese
	/// @brief 頂点バッファの配列を設定します。
	void SetVertexBuffers(std::vector<std::shared_ptr<VertexBuffer>> && vertexBuffers);

	///@~Japanese
	/// @brief 指定されたテクスチャスロットにテクスチャを取得します。
	/// @param index 0 から始まるテクスチャスロットのインデックスを指定します。
	std::shared_ptr<Texture> GetTexture(std::uint32_t index) const;

	///@~Japanese
	/// @brief グラフィックスコンテキストに設定されているテクスチャをテクスチャユニットから外します。
	/// @param index 0 から始まるテクスチャスロットのインデックスを指定します。
	void SetTexture(std::uint32_t index);

	///@~Japanese
	/// @brief 指定されたテクスチャスロットにテクスチャを設定します。
	/// @param index 0 から始まるテクスチャスロットのインデックスを指定します。
	/// @param texture テクスチャスロットにバインドするテクスチャ
	void SetTexture(std::uint32_t index, std::shared_ptr<Texture2D> const& texture);
	
	///@~Japanese
	/// @brief 指定されたテクスチャスロットにテクスチャを設定します。
	/// @param index 0 から始まるテクスチャスロットのインデックスを指定します。
	/// @param texture テクスチャスロットにバインドするテクスチャ
	void SetTexture(std::uint32_t index, std::shared_ptr<RenderTarget2D> const& texture);

	///@~Japanese
	/// @brief レンダーターゲットを取得します。
	std::vector<std::shared_ptr<RenderTarget2D>> GetRenderTargets() const;

	///@~Japanese
	/// @brief レンダーターゲットを設定します。
	/// @details 引数に何も指定していない場合、現在設定されているすべてのレンダーターゲットを取り外します。
	void SetRenderTarget();

	///@~Japanese
	/// @brief レンダーターゲットを設定します。
	void SetRenderTarget(std::shared_ptr<RenderTarget2D> const& renderTarget);

	///@~Japanese
	/// @brief 複数のレンダーターゲットを設定します。
	void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargets);

	///@~Japanese
	/// @brief 複数のレンダーターゲットを設定します。
	void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> && renderTargets);

	///@~Japanese
	/// @brief エフェクトパスを設定します。
	void SetEffectPass(std::shared_ptr<EffectPass> const& effectPass);

	///@~Japanese
	/// @brief 定数バッファを設定します。
	void SetConstantBuffers(std::shared_ptr<ConstantBufferBinding> const& constantBuffers);

public:
	Details::RenderSystem::NativeGraphicsContext* NativeGraphicsContext();
	
private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace Pomdog

#endif // !defined(POMDOG_GRAPHICSCONTEXT_F873DD34_5F2E_44DD_A419_0E78FE2D56E5_HPP)
