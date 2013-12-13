//
//  Copyright (C) 2013 mogemimi.
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
		Details::RenderSystem::PresentationParameters const& presentationParameters);
	
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

public:
	//Details::RenderSystem::NativeGraphicsContext* NativeGraphicsContext();
	
private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_GRAPHICSCONTEXT_F873DD34_5F2E_44DD_A419_0E78FE2D56E5_HPP)
