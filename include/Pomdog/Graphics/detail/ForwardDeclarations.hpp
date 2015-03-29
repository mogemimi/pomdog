// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_FORWARDDECLARATIONS_1D9B55F8_HPP
#define POMDOG_FORWARDDECLARATIONS_1D9B55F8_HPP

#include <cstdint>

namespace Pomdog {

// Classes
class ConstantBufferBinding;
class EffectParameter;
class EffectPass;
class EffectReflection;
class GraphicsContext;
class GraphicsDevice;
class IndexBuffer;
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
class EffectConstantDescription;
class EffectPassDescription;
class EffectVariable;
class InputElement;
class InputLayoutDescription;
class RasterizerDescription;
class SamplerDescription;
class Viewport;

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
enum class InputClassification: std::uint8_t;
enum class InputElementFormat: std::uint8_t;
enum class PrimitiveTopology: std::uint8_t;
enum class ShaderLanguage: std::uint8_t;
enum class StencilOperation: std::uint8_t;
enum class SurfaceFormat: std::uint8_t;
enum class TextureAddressMode: std::uint8_t;
enum class TextureFilter: std::uint8_t;

} // namespace Pomdog

#endif // POMDOG_FORWARDDECLARATIONS_1D9B55F8_HPP
