// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

class NativeBuffer;

class ShaderBytecode;
struct ShaderCompileOptions;

} // namespace pomdog::detail

namespace pomdog {

// Classes
class ConstantBuffer;
class DepthStencilBuffer;
class EffectReflection;
class GraphicsCommandList;
class GraphicsCommandQueue;
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
struct RenderPass;
struct RenderTargetBlendDescription;
struct SamplerDescription;

// Enumerations
enum class BlendFactor : std::uint8_t;
enum class BlendOperation : std::uint8_t;
enum class BufferUsage : std::uint8_t;
enum class ComparisonFunction : std::uint8_t;
enum class CullMode : std::uint8_t;
enum class EffectVariableClass : std::uint8_t;
enum class EffectVariableType : std::uint8_t;
enum class FillMode : std::uint8_t;
enum class IndexElementSize : std::uint8_t;
enum class InputClassification : std::uint8_t;
enum class InputElementFormat : std::uint8_t;
enum class PrimitiveTopology : std::uint8_t;
enum class ShaderLanguage : std::uint8_t;
enum class StencilOperation : std::uint8_t;
enum class SurfaceFormat : std::uint8_t;
enum class TextureAddressMode : std::uint8_t;
enum class TextureFilter : std::uint8_t;

} // namespace pomdog
