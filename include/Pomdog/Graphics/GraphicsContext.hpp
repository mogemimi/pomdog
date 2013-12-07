//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_GRAPHICSCONTEXT_HPP
#define POMDOG_GRAPHICSCONTEXT_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"
#include "../Utility/Noncopyable.hpp"
#include "../Math/detail/ForwardDeclarations.hpp"
#include "detail/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeGraphicsContext;

}// namespace RenderSystem
}// namespace Details

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief 下位レベルの API を提供するグラフィックスコンテキストです。レンダリングコマンドを生成するために使用します。
class GraphicsContext: Noncopyable
{
public:
	GraphicsContext() = delete;

	explicit GraphicsContext(std::unique_ptr<Details::RenderSystem::NativeGraphicsContext> nativeContext);
	virtual ~GraphicsContext();
	
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
	
private:
	//friend class Details::RenderSystem::InternalAttorney;
	//Details::RenderSystem::NativeGraphicsContext* NativeGraphicsContext();
	
private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_GRAPHICSCONTEXT_HPP)
