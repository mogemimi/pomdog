// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

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
struct InputElement;
struct InputLayoutDescriptor;
struct PipelineDescriptor;
struct PresentationParameters;
struct RasterizerDescriptor;
struct RenderPass;
struct RenderTargetBlendDescriptor;
struct SamplerDescriptor;

enum class BlendFactor : u8;
enum class BlendOperation : u8;
enum class BufferUsage : u8;
enum class ComparisonFunction : u8;
enum class CullMode : u8;
enum class FillMode : u8;
enum class IndexFormat : u8;
enum class InputClassification : u8;
enum class InputElementFormat : u8;
enum class PrimitiveTopology : u8;
enum class StencilOperation : u8;
enum class TextureAddressMode : u8;
enum class TextureFilter : u8;

} // namespace pomdog::gpu
