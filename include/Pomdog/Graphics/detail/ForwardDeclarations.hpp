// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_DETAIL_GRAPHICS_FORWARDDECLARATIONS_1D9B55F8_7EC3_4561_BA79_7F348F825F5A_HPP
#define POMDOG_DETAIL_GRAPHICS_FORWARDDECLARATIONS_1D9B55F8_7EC3_4561_BA79_7F348F825F5A_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

// Classes
class BlendState;
class ConstantBufferBinding;
class DepthStencilState;
class EffectConstantDescription;
class EffectParameter;
class EffectPass;
class EffectPassDescription;
class EffectReflection;
class GraphicsContext;
class GraphicsDevice;
class IndexBuffer;
class RasterizerState;
class RenderTarget2D;
class SamplerState;
class Shader;
class Texture;
class Texture2D;
class VertexBuffer;

// Structures
class BlendDescription;
class DepthStencilDescription;
class DepthStencilOperation;
class EffectAnnotation;
class EffectVariable;
class RasterizerDescription;
class SamplerDescription;
class VertexDeclaration;
class VertexElement;
class Viewport;
class VertexBufferBinding;

// Enumerations
enum class Blend: std::uint8_t;
enum class BlendFunction: std::uint8_t;
enum class BufferUsage: std::uint8_t;
enum class ClearOptions: std::uint8_t;
enum class ComparisonFunction: std::uint8_t;
enum class CullMode: std::uint8_t;
enum class DepthFormat: std::uint8_t;
enum class EffectVariableClass: std::uint8_t;
enum class EffectVariableType: std::uint8_t;
enum class FillMode: std::uint8_t;
enum class IndexElementSize: std::uint8_t;
enum class PrimitiveTopology: std::uint8_t;
enum class ShaderLanguage: std::uint8_t;
enum class StencilOperation: std::uint8_t;
enum class SurfaceFormat: std::uint8_t;
enum class TextureAddressMode: std::uint8_t;
enum class TextureFilter: std::uint8_t;
enum class VertexElementFormat: std::uint8_t;

}// namespace Pomdog

#endif // !defined(POMDOG_DETAIL_GRAPHICS_FORWARDDECLARATIONS_1D9B55F8_7EC3_4561_BA79_7F348F825F5A_HPP)
