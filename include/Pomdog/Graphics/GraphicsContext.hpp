//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_GRAPHICSCONTEXT_F873DD34_5F2E_44DD_A419_0E78FE2D56E5_HPP
#define POMDOG_GRAPHICSCONTEXT_F873DD34_5F2E_44DD_A419_0E78FE2D56E5_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include <vector>
#include "../Config/FundamentalTypes.hpp"
#include "../Config/Export.hpp"
#include "../Utility/Noncopyable.hpp"
#include "../Math/detail/ForwardDeclarations.hpp"
#include "detail/ForwardDeclarations.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeGraphicsContext;
class PresentationParameters;

}// namespace RenderSystem
}// namespace Details

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief 下位レベルの API を提供するグラフィックスコンテキストです。レンダリングコマンドを生成するために使用します。
class POMDOG_EXPORT GraphicsContext: Noncopyable
{
public:
	GraphicsContext() = delete;

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
	/// @param primitiveTopology プリミティブの位相を指定します。
	void Draw(PrimitiveTopology primitiveTopology);

	///@~Japanese
	/// @brief インデックス付けされた頂点データに基づき、インスタンス化されていないプリミティブを描画します。
	/// @param primitiveTopology プリミティブの位相を指定します。
	/// @param indexBuffer インデックスバッファを指定します。
	/// @param indexCount インデックスの要素数を指定します。
	void DrawIndexed(PrimitiveTopology primitiveTopology,
		std::shared_ptr<IndexBuffer> const& indexBuffer, std::uint32_t indexCount);

	///@~Japanese
	/// @brief インデックスを持たない、インスタンス化されたプリミティブを描画します。
	/// @param primitiveTopology プリミティブの位相を指定します。
	/// @param instanceCount レンダリングするプリミティブのインスタンスの数を指定します。
	void DrawInstanced(PrimitiveTopology primitiveTopology, std::uint32_t instanceCount);

	///@~Japanese
	/// @brief インデックス付けされた頂点データに基づき、インスタンス化されたプリミティブを描画します。
	/// @param primitiveTopology プリミティブの位相を指定します。
	/// @param indexBuffer インデックスバッファを指定します。
	/// @param indexCount インデックスの要素数を指定します。
	/// @param instanceCount レンダリングするプリミティブのインスタンスの数を指定します。
	void DrawIndexedInstanced(PrimitiveTopology primitiveTopology,
		std::shared_ptr<IndexBuffer> const& indexBuffer, std::uint32_t indexCount, std::uint32_t instanceCount);
	
	///@~Japanese
	/// @brief ビューポートを取得します。
	Viewport const& GetViewport() const;

	///@~Japanese
	/// @brief ビューポートを設定します。
	void SetViewport(Viewport const& viewport);
	
	///@~Japanese
	/// @brief シザーテストで用いるシザー矩形を取得します。
	Rectangle GetScissorRectangle() const;
	
	///@~Japanese
	/// @brief シザーテストで用いるシザー矩形を設定します。
	void SetScissorRectangle(Rectangle const& rectangle);
	
	///@~Japanese
	/// @brief ブレンディングステートを設定します。
	void SetBlendState(std::shared_ptr<BlendState> const& blendState);

	///@~Japanese
	/// @brief 深度ステンシルステートを設定します。
	void SetDepthStencilState(std::shared_ptr<DepthStencilState> const& depthStencilState);

	///@~Japanese
	/// @brief ラスタライザーステートを設定します。
	void SetRasterizerState(std::shared_ptr<RasterizerState> const& rasterizerState);

	///@~Japanese
	/// @brief テクスチャサンプラーステートを設定します。
	void SetSamplerState(std::size_t index, std::shared_ptr<SamplerState> const& samplerState);

	///@~Japanese
	/// @brief 入力レイアウトを設定します。
	void SetInputLayout(std::shared_ptr<InputLayout> const& inputLayout);

	///@~Japanese
	/// @brief 頂点バッファを設定します。
	void SetVertexBuffer(std::shared_ptr<VertexBuffer> const& vertexBuffer);
	
	///@~Japanese
	/// @brief 頂点バッファの配列を設定します。
	void SetVertexBuffers(std::vector<std::shared_ptr<VertexBuffer>> const& vertexBuffers);

public:
	Details::RenderSystem::NativeGraphicsContext* GetNativeGraphicsContext();
	
private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_GRAPHICSCONTEXT_F873DD34_5F2E_44DD_A419_0E78FE2D56E5_HPP)
