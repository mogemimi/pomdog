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
#include <memory>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeGraphicsContext;
class InternalAttorney;

}// namespace RenderSystem
}// namespace Details

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

class GraphicsContext: Noncopyable
{
public:
	explicit GraphicsContext(std::unique_ptr<Details::RenderSystem::NativeGraphicsContext> nativeContext);
	virtual ~GraphicsContext();
	
	///@~Japanese
	/// @brief 現在バインドされているレンダーターゲットを指定された色でクリアーします。
	void Clear(Color const& color);
	
	///@~Japanese
	/// @brief バックバッファの内容をディスプレイに表示します。
	void Present();
	
private:
	friend class Details::RenderSystem::InternalAttorney;
	Details::RenderSystem::NativeGraphicsContext* NativeContext();
	
private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_GRAPHICSCONTEXT_HPP)
