//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_GL4_GRAPHICSDEVICEGL4_74F5BC81_6BDB_4FDC_99F4_DF45CEC32B5F_HPP
#define POMDOG_SRC_GL4_GRAPHICSDEVICEGL4_74F5BC81_6BDB_4FDC_99F4_DF45CEC32B5F_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include <memory>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

class GraphicsDeviceGL4 final: public NativeGraphicsDevice
{
public:
	GraphicsDeviceGL4() = default;

	~GraphicsDeviceGL4() = default;
	
	///@copydoc NativeGraphicsDevice
	std::unique_ptr<NativeIndexBuffer>
	CreateIndexBuffer(void const* indices, std::size_t indexCount,
		IndexElementSize elementSize, BufferUsage bufferUsage) override;
	
	///@copydoc NativeGraphicsDevice
	std::unique_ptr<NativeVertexBuffer>
	CreateVertexBuffer(void const* vertices, std::size_t vertexCount,
		VertexDeclaration const& vertexDeclaration, BufferUsage bufferUsage) override;

	///@copydoc NativeGraphicsDevice
	std::unique_ptr<NativeBlendState>
	CreateBlendState(BlendDescription const& description) override;

	///@copydoc NativeGraphicsDevice
	std::unique_ptr<NativeDepthStencilState>
	CreateDepthStencilState(DepthStencilDescription const& description) override;

	///@copydoc NativeGraphicsDevice
	std::unique_ptr<NativeSamplerState>
	CreateSamplerState(SamplerDescription const& description) override;

	///@copydoc NativeGraphicsDevice
	std::unique_ptr<NativeRasterizerState>
	CreateRasterizerState(RasterizerDescription const& description) override;
	
	///@copydoc NativeGraphicsDevice
	std::unique_ptr<NativeEffectPass>
	CreateEffectPass(ShaderBytecode const& vertexShaderBytecode,
		ShaderBytecode const& pixelShaderBytecode) override;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_GL4_GRAPHICSDEVICEGL4_74F5BC81_6BDB_4FDC_99F4_DF45CEC32B5F_HPP)
