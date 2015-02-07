//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_PREREQUISITESDIRECT3D11_EED05C73_4879_45F8_B1EB_0AABCC8FBCD5_HPP
#define POMDOG_PREREQUISITESDIRECT3D11_EED05C73_4879_45F8_B1EB_0AABCC8FBCD5_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <wrl/client.h> // ComPtr

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace Direct3D11 {

class BlendStateDirect3D11;
class ConstantBufferDirect3D11;
class ConstantLayoutDirect3D11;
class DepthStencilStateDirect3D11;
class EffectPassDirect3D11;
class EffectReflectionDirect3D11;
class GraphicsDeviceDirect3D11;
class GraphicsContextDirect3D11;
class IndexBufferDirect3D11;
class RasterizerStateDirect3D11;
class RenderTarget2DDirect3D11;
class SamplerStateDirect3D11;
class Texture2DDirect3D11;
class VertexBufferDirect3D11;

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Details
}// namespace Readymade

#endif // !defined(POMDOG_PREREQUISITESDIRECT3D11_EED05C73_4879_45F8_B1EB_0AABCC8FBCD5_HPP)
