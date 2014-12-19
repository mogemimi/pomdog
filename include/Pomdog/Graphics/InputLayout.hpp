//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_INPUTLAYOUT_B0E2CBE1_3C0D_4D11_8260_CF62EE25D392_HPP
#define POMDOG_INPUTLAYOUT_B0E2CBE1_3C0D_4D11_8260_CF62EE25D392_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <initializer_list>
#include <vector>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeInputLayout;

}// namespace RenderSystem
}// namespace Details

///@~Japanese
/// @brief 入力レイアウトを記述します。
class POMDOG_EXPORT InputLayout {
public:
	InputLayout() = delete;
	InputLayout(InputLayout const&) = delete;
	InputLayout & operator=(InputLayout const&) = delete;
	
	InputLayout(InputLayout &&) = default;
	InputLayout & operator=(InputLayout &&) = default;
	
	InputLayout(GraphicsDevice & graphicsDevice,
		std::shared_ptr<EffectPass> const& effectPass,
		VertexDeclaration const& vertexDeclaration);

	InputLayout(GraphicsDevice & graphicsDevice,
		std::shared_ptr<EffectPass> const& effectPass,
		std::vector<VertexBufferBinding> const& vertexBindings);
	
	InputLayout(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
		std::shared_ptr<EffectPass> const& effectPass,
		VertexDeclaration const& vertexDeclaration);

	InputLayout(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
		std::shared_ptr<EffectPass> const& effectPass,
		std::vector<VertexBufferBinding> const& vertexBindings);

	~InputLayout();

public:
	Details::RenderSystem::NativeInputLayout* NativeInputLayout();

private:
	std::unique_ptr<Details::RenderSystem::NativeInputLayout> nativeInputLayout;
};

}// namespace Pomdog

#endif // !defined(POMDOG_INPUTLAYOUT_B0E2CBE1_3C0D_4D11_8260_CF62EE25D392_HPP)
