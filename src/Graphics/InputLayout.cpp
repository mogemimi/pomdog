//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Pomdog/Graphics/InputLayout.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeInputLayout.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/VertexBufferBinding.hpp"
#include "Pomdog/Graphics/EffectPass.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
InputLayout::InputLayout(GraphicsDevice & graphicsDevice,
	std::shared_ptr<EffectPass> const& effectPass,
	std::vector<VertexBufferBinding> const& vertexBindings)
{
	auto nativeEffectPass = effectPass->NativeEffectPass();
	POMDOG_ASSERT(nativeEffectPass != nullptr);

	auto nativeDevice = graphicsDevice.NativeGraphicsDevice();
	nativeInputLayout = nativeDevice->CreateInputLayout(*nativeEffectPass, vertexBindings);
}
//-----------------------------------------------------------------------
InputLayout::InputLayout(GraphicsDevice & graphicsDevice,
	std::shared_ptr<EffectPass> const& effectPass,
	VertexDeclaration const& vertexDeclaration)
	: InputLayout(graphicsDevice, effectPass, std::initializer_list<VertexBufferBinding>{
		VertexBufferBinding{vertexDeclaration, 0, 0}})
{}
//-----------------------------------------------------------------------
InputLayout::InputLayout(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	std::shared_ptr<EffectPass> const& effectPass,
	std::vector<VertexBufferBinding> const& vertexBindings)
	: InputLayout(*graphicsDevice, effectPass, vertexBindings)
{}
//-----------------------------------------------------------------------
InputLayout::InputLayout(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	std::shared_ptr<EffectPass> const& effectPass,
	VertexDeclaration const& vertexDeclaration)
	: InputLayout(*graphicsDevice, effectPass, std::initializer_list<VertexBufferBinding>{
		VertexBufferBinding{vertexDeclaration, 0, 0}})
{}
//-----------------------------------------------------------------------
InputLayout::~InputLayout() = default;
//-----------------------------------------------------------------------
Details::RenderSystem::NativeInputLayout* InputLayout::NativeInputLayout()
{
	return nativeInputLayout.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
