// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail {
class ShaderBytecode;
struct ShaderCompileOptions;
} // namespace pomdog::gpu::detail

namespace pomdog::gpu {

class Buffer;
class CommandList;
class CommandQueue;
class ConstantBuffer;
class DepthStencilBuffer;
class EffectReflection;
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

struct BlendDescriptor;
struct DepthStencilDescriptor;
struct DepthStencilOperation;
struct EffectAnnotation;
struct EffectConstantDescription;
struct EffectVariable;
struct InputElement;
struct InputLayoutDescriptor;
struct PipelineDescriptor;
struct RasterizerDescriptor;
struct RenderPass;
struct RenderTargetBlendDescriptor;
struct SamplerDescriptor;

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
enum class TextureAddressMode : std::uint8_t;
enum class TextureFilter : std::uint8_t;

} // namespace pomdog::gpu

namespace pomdog {
struct PresentationParameters;
enum class PixelFormat : std::uint8_t;
} // namespace pomdog
