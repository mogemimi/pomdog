//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DETAIL_GRAPHICS_FORWARDDECLARATIONS_1D9B55F8_7EC3_4561_BA79_7F348F825F5A_HPP
#define POMDOG_DETAIL_GRAPHICS_FORWARDDECLARATIONS_1D9B55F8_7EC3_4561_BA79_7F348F825F5A_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Config/FundamentalTypes.hpp>

namespace Pomdog {

// Class
class BlendDescription;
class BlendState;
template <typename T, typename... Arguments>
class CustomVertex;
class DepthStencilDescription;
class DepthStencilOperation;
class DepthStencilState;
class DynamicIndexBuffer;
class DynamicVertexBuffer;
class EffectAnnotation;
class EffectParameter;
class EffectPass;
class GraphicsContext;
class GraphicsDevice;
class IndexBuffer;
class InputLayout;
class RasterizerDescription;
class RasterizerState;
class SamplerDescription;
class SamplerState;
class VertexBuffer;
class VertexBufferBinding;
class VertexDeclaration;
class VertexElement;
class Viewport;

// Enumerations
enum class Blend: std::uint8_t;
enum class BlendFunction: std::uint8_t;
enum class BufferUsage: std::uint8_t;
enum class ClearOptions: std::uint8_t;
enum class ComparisonFunction: std::uint8_t;
enum class CullMode: std::uint8_t;
enum class DepthFormat: std::uint8_t;
enum class EffectParameterClass: std::uint8_t;
enum class EffectParameterType: std::uint8_t;
enum class FillMode: std::uint8_t;
enum class IndexElementSize: std::uint8_t;
enum class PrimitiveTopology: std::uint8_t;
enum class StencilOperation: std::uint8_t;
enum class TextureAddressMode: std::uint8_t;
enum class TextureFilter: std::uint8_t;
enum class VertexElementFormat: std::uint8_t;

}// namespace Pomdog

#endif // !defined(POMDOG_DETAIL_GRAPHICS_FORWARDDECLARATIONS_1D9B55F8_7EC3_4561_BA79_7F348F825F5A_HPP)
