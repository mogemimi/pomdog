// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_FORWARDDECLARATIONS_1D9B55F8_HPP
#define POMDOG_FORWARDDECLARATIONS_1D9B55F8_HPP

#include <cstdint>

namespace Pomdog {
namespace Detail {

class NativeBuffer;
class NativeConstantLayout;
class NativeEffectReflection;
class NativeGraphicsContext;
class NativeGraphicsDevice;
class NativePipelineState;
class NativeRenderTarget2D;
class NativeSamplerState;
class NativeTexture2D;

} // namespace Detail

// Classes
class ConstantBuffer;
class ConstantBufferBinding;
class EffectReflection;
class GraphicsContext;
class GraphicsDevice;
class IndexBuffer;
class PipelineState;
class RenderTarget2D;
class SamplerState;
class Shader;
class Texture;
class Texture2D;
class VertexBuffer;
class Viewport;

// Structures
struct BlendDescription;
struct DepthStencilDescription;
struct DepthStencilOperation;
struct EffectAnnotation;
struct EffectConstantDescription;
struct EffectVariable;
struct InputElement;
struct InputLayoutDescription;
struct PipelineStateDescription;
struct PresentationParameters;
struct RasterizerDescription;
struct RenderTargetBlendDescription;
struct SamplerDescription;

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
enum class PrimitiveTopologyType : std::uint8_t;
enum class ShaderLanguage: std::uint8_t;
enum class StencilOperation: std::uint8_t;
enum class SurfaceFormat: std::uint8_t;
enum class TextureAddressMode: std::uint8_t;
enum class TextureFilter: std::uint8_t;

} // namespace Pomdog

#endif // POMDOG_FORWARDDECLARATIONS_1D9B55F8_HPP
