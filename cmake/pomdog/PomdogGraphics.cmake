source_group(Graphics                   REGULAR_EXPRESSION "(include/Pomdog|src)/Graphics/*")
source_group(Graphics\\ShaderCompilers  REGULAR_EXPRESSION "(include/Pomdog|src)/Graphics/ShaderCompilers/*")
source_group(Graphics.Backends          REGULAR_EXPRESSION "(include/Pomdog|src)/Graphics.Backends/*")
source_group(Graphics.DXGI              REGULAR_EXPRESSION "(include/Pomdog|src)/Graphics.DXGI/*")
source_group(Graphics.Direct3D          REGULAR_EXPRESSION "(include/Pomdog|src)/Graphics.Direct3D/*")
source_group(Graphics.Direct3D11        REGULAR_EXPRESSION "(include/Pomdog|src)/Graphics.Direct3D11/*")
source_group(Graphics.GL4               REGULAR_EXPRESSION "(include/Pomdog|src)/Graphics.GL4/*")
source_group(Graphics.Metal             REGULAR_EXPRESSION "(include/Pomdog|src)/Graphics.Metal/*")
source_group(Graphics.Vulkan            REGULAR_EXPRESSION "(include/Pomdog|src)/Graphics.Vulkan/*")

target_sources(pomdog_static PRIVATE
    # NOTE: Graphics
    ${POMDOG_INC_DIR}/Graphics/Blend.hpp
    ${POMDOG_INC_DIR}/Graphics/BlendDescription.hpp
    ${POMDOG_INC_DIR}/Graphics/BlendOperation.hpp
    ${POMDOG_INC_DIR}/Graphics/BufferUsage.hpp
    ${POMDOG_INC_DIR}/Graphics/ComparisonFunction.hpp
    ${POMDOG_INC_DIR}/Graphics/ConstantBuffer.hpp
    ${POMDOG_SRC_DIR}/Graphics/ConstantBuffer.cpp
    ${POMDOG_INC_DIR}/Graphics/CullMode.hpp
    ${POMDOG_INC_DIR}/Graphics/DepthStencilBuffer.hpp
    ${POMDOG_SRC_DIR}/Graphics/DepthStencilBuffer.cpp
    ${POMDOG_INC_DIR}/Graphics/DepthStencilDescription.hpp
    ${POMDOG_INC_DIR}/Graphics/DepthStencilOperation.hpp
    ${POMDOG_INC_DIR}/Graphics/EffectAnnotation.hpp
    ${POMDOG_INC_DIR}/Graphics/EffectConstantDescription.hpp
    ${POMDOG_INC_DIR}/Graphics/EffectReflection.hpp
    ${POMDOG_SRC_DIR}/Graphics/EffectReflection.cpp
    ${POMDOG_INC_DIR}/Graphics/EffectVariableClass.hpp
    ${POMDOG_INC_DIR}/Graphics/EffectVariableType.hpp
    ${POMDOG_INC_DIR}/Graphics/EffectVariable.hpp
    ${POMDOG_INC_DIR}/Graphics/FillMode.hpp
    ${POMDOG_INC_DIR}/Graphics/ForwardDeclarations.hpp
    ${POMDOG_INC_DIR}/Graphics/GraphicsCommandList.hpp
    ${POMDOG_SRC_DIR}/Graphics/GraphicsCommandList.cpp
    ${POMDOG_INC_DIR}/Graphics/GraphicsCommandQueue.hpp
    ${POMDOG_SRC_DIR}/Graphics/GraphicsCommandQueue.cpp
    ${POMDOG_INC_DIR}/Graphics/GraphicsDevice.hpp
    ${POMDOG_SRC_DIR}/Graphics/GraphicsDevice.cpp
    ${POMDOG_INC_DIR}/Graphics/IndexBuffer.hpp
    ${POMDOG_SRC_DIR}/Graphics/IndexBuffer.cpp
    ${POMDOG_INC_DIR}/Graphics/IndexElementSize.hpp
    ${POMDOG_INC_DIR}/Graphics/InputClassification.hpp
    ${POMDOG_INC_DIR}/Graphics/InputElement.hpp
    ${POMDOG_INC_DIR}/Graphics/InputElementFormat.hpp
    ${POMDOG_INC_DIR}/Graphics/InputLayoutDescription.hpp
    ${POMDOG_INC_DIR}/Graphics/InputLayoutHelper.hpp
    ${POMDOG_SRC_DIR}/Graphics/InputLayoutHelper.cpp
    ${POMDOG_INC_DIR}/Graphics/PipelineState.hpp
    ${POMDOG_SRC_DIR}/Graphics/PipelineState.cpp
    ${POMDOG_INC_DIR}/Graphics/PipelineStateDescription.hpp
    ${POMDOG_INC_DIR}/Graphics/PresentationParameters.hpp
    ${POMDOG_INC_DIR}/Graphics/PrimitiveTopology.hpp
    ${POMDOG_INC_DIR}/Graphics/RasterizerDescription.hpp
    ${POMDOG_INC_DIR}/Graphics/RenderPass.hpp
    ${POMDOG_INC_DIR}/Graphics/RenderTarget2D.hpp
    ${POMDOG_SRC_DIR}/Graphics/RenderTarget2D.cpp
    ${POMDOG_INC_DIR}/Graphics/RenderTargetBlendDescription.hpp
    ${POMDOG_INC_DIR}/Graphics/SamplerDescription.hpp
    ${POMDOG_INC_DIR}/Graphics/SamplerState.hpp
    ${POMDOG_SRC_DIR}/Graphics/SamplerState.cpp
    ${POMDOG_INC_DIR}/Graphics/Shader.hpp
    ${POMDOG_SRC_DIR}/Graphics/Shader.cpp
    ${POMDOG_INC_DIR}/Graphics/ShaderLanguage.hpp
    ${POMDOG_INC_DIR}/Graphics/ShaderPipelineStage.hpp
    ${POMDOG_INC_DIR}/Graphics/SurfaceFormat.hpp
    ${POMDOG_INC_DIR}/Graphics/StencilOperation.hpp
    ${POMDOG_INC_DIR}/Graphics/Texture.hpp
    ${POMDOG_SRC_DIR}/Graphics/Texture.cpp
    ${POMDOG_INC_DIR}/Graphics/Texture2D.hpp
    ${POMDOG_SRC_DIR}/Graphics/Texture2D.cpp
    ${POMDOG_INC_DIR}/Graphics/TextureAddressMode.hpp
    ${POMDOG_INC_DIR}/Graphics/TextureFilter.hpp
    ${POMDOG_INC_DIR}/Graphics/VertexBuffer.hpp
    ${POMDOG_SRC_DIR}/Graphics/VertexBuffer.cpp
    ${POMDOG_INC_DIR}/Graphics/Viewport.hpp
    ${POMDOG_SRC_DIR}/Graphics/Viewport.cpp
    ${POMDOG_INC_DIR}/Graphics/ShaderCompilers/GLSLCompiler.hpp
    ${POMDOG_SRC_DIR}/Graphics/ShaderCompilers/GLSLCompiler.cpp
    ${POMDOG_INC_DIR}/Graphics/ShaderCompilers/HLSLCompiler.hpp
    ${POMDOG_SRC_DIR}/Graphics/ShaderCompilers/HLSLCompiler.cpp
    ${POMDOG_INC_DIR}/Graphics/ShaderCompilers/MetalCompiler.hpp
    ${POMDOG_SRC_DIR}/Graphics/ShaderCompilers/MetalCompiler.cpp

    # NOTE: Graphics.Backends
    ${POMDOG_SRC_DIR}/Graphics.Backends/BufferBindMode.hpp
    ${POMDOG_SRC_DIR}/Graphics.Backends/BufferHelper.cpp
    ${POMDOG_SRC_DIR}/Graphics.Backends/BufferHelper.hpp
    ${POMDOG_SRC_DIR}/Graphics.Backends/GraphicsCapabilities.hpp
    ${POMDOG_SRC_DIR}/Graphics.Backends/GraphicsCommandListImmediate.cpp
    ${POMDOG_SRC_DIR}/Graphics.Backends/GraphicsCommandListImmediate.hpp
    ${POMDOG_SRC_DIR}/Graphics.Backends/GraphicsCommandQueueImmediate.cpp
    ${POMDOG_SRC_DIR}/Graphics.Backends/GraphicsCommandQueueImmediate.hpp
    ${POMDOG_SRC_DIR}/Graphics.Backends/NativeBuffer.hpp
    ${POMDOG_SRC_DIR}/Graphics.Backends/NativeBuffer.cpp
    ${POMDOG_SRC_DIR}/Graphics.Backends/NativeGraphicsContext.cpp
    ${POMDOG_SRC_DIR}/Graphics.Backends/NativeGraphicsContext.hpp
    ${POMDOG_SRC_DIR}/Graphics.Backends/ShaderBytecode.hpp
    ${POMDOG_SRC_DIR}/Graphics.Backends/ShaderCompileOptions.hpp
    ${POMDOG_SRC_DIR}/Graphics.Backends/SurfaceFormatHelper.cpp
    ${POMDOG_SRC_DIR}/Graphics.Backends/SurfaceFormatHelper.hpp
    ${POMDOG_SRC_DIR}/Graphics.Backends/TextureHelper.cpp
    ${POMDOG_SRC_DIR}/Graphics.Backends/TextureHelper.hpp

    $<$<AND:$<PLATFORM_ID:Windows,Linux,Darwin>,$<BOOL:${POMDOG_USE_GL4}>>:
        # NOTE: Graphics.GL4
        ${POMDOG_SRC_DIR}/Graphics.GL4/BlendStateGL4.cpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/BlendStateGL4.hpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/BufferGL4.cpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/BufferGL4.hpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/DepthStencilBufferGL4.cpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/DepthStencilBufferGL4.hpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/DepthStencilStateGL4.cpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/DepthStencilStateGL4.hpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/EffectReflectionGL4.cpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/EffectReflectionGL4.hpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/ErrorChecker.cpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/ErrorChecker.hpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/FormatHelper.cpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/FormatHelper.hpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/GraphicsContextGL4.cpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/GraphicsContextGL4.hpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/GraphicsDeviceGL4.cpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/GraphicsDeviceGL4.hpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/InputLayoutGL4.cpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/InputLayoutGL4.hpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/OpenGLContext.hpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/OpenGLContext.cpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/OpenGLPrerequisites.hpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/PipelineStateGL4.cpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/PipelineStateGL4.hpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/RasterizerStateGL4.cpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/RasterizerStateGL4.hpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/RenderTarget2DGL4.cpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/RenderTarget2DGL4.hpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/SamplerStateGL4.cpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/SamplerStateGL4.hpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/ShaderGL4.cpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/ShaderGL4.hpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/Texture2DGL4.cpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/Texture2DGL4.hpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/TypesafeGL4.hpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/TypesafeHelperGL4.hpp
        ${POMDOG_SRC_DIR}/Graphics.GL4/VertexBufferBindingGL4.hpp
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: Graphics.DXGI
        ${POMDOG_SRC_DIR}/Graphics.DXGI/DXGIFormatHelper.cpp
        ${POMDOG_SRC_DIR}/Graphics.DXGI/DXGIFormatHelper.hpp
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: Graphics.Direct3D
        ${POMDOG_SRC_DIR}/Graphics.Direct3D/HLSLCompiling.cpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D/HLSLCompiling.hpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D/HLSLReflectionHelper.cpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D/HLSLReflectionHelper.hpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D/PrerequisitesDirect3D.hpp
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: Graphics.Direct3D11
        ${POMDOG_SRC_DIR}/Graphics.Direct3D11/BufferDirect3D11.cpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D11/BufferDirect3D11.hpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D11/DepthStencilBufferDirect3D11.cpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D11/DepthStencilBufferDirect3D11.hpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D11/EffectReflectionDirect3D11.cpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D11/EffectReflectionDirect3D11.hpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D11/FormatHelper.cpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D11/FormatHelper.hpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D11/GraphicsContextDirect3D11.cpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D11/GraphicsContextDirect3D11.hpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D11/GraphicsDeviceDirect3D11.cpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D11/GraphicsDeviceDirect3D11.hpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D11/PipelineStateDirect3D11.cpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D11/PipelineStateDirect3D11.hpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D11/PrerequisitesDirect3D11.hpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D11/RenderTarget2DDirect3D11.cpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D11/RenderTarget2DDirect3D11.hpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D11/SamplerStateDirect3D11.cpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D11/SamplerStateDirect3D11.hpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D11/ShaderDirect3D11.cpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D11/ShaderDirect3D11.hpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D11/Texture2DDirect3D11.cpp
        ${POMDOG_SRC_DIR}/Graphics.Direct3D11/Texture2DDirect3D11.hpp
    >

    $<$<PLATFORM_ID:Darwin>:
        # NOTE: Graphics.Metal
        ${POMDOG_SRC_DIR}/Graphics.Metal/BufferMetal.hpp
        ${POMDOG_SRC_DIR}/Graphics.Metal/BufferMetal.mm
        ${POMDOG_SRC_DIR}/Graphics.Metal/ConstantsMetal.hpp
        ${POMDOG_SRC_DIR}/Graphics.Metal/DepthStencilBufferMetal.hpp
        ${POMDOG_SRC_DIR}/Graphics.Metal/DepthStencilBufferMetal.mm
        ${POMDOG_SRC_DIR}/Graphics.Metal/EffectReflectionMetal.hpp
        ${POMDOG_SRC_DIR}/Graphics.Metal/EffectReflectionMetal.mm
        ${POMDOG_SRC_DIR}/Graphics.Metal/GraphicsContextMetal.hpp
        ${POMDOG_SRC_DIR}/Graphics.Metal/GraphicsContextMetal.mm
        ${POMDOG_SRC_DIR}/Graphics.Metal/GraphicsDeviceMetal.hpp
        ${POMDOG_SRC_DIR}/Graphics.Metal/GraphicsDeviceMetal.mm
        ${POMDOG_SRC_DIR}/Graphics.Metal/MetalFormatHelper.hpp
        ${POMDOG_SRC_DIR}/Graphics.Metal/MetalFormatHelper.mm
        ${POMDOG_SRC_DIR}/Graphics.Metal/PipelineStateMetal.hpp
        ${POMDOG_SRC_DIR}/Graphics.Metal/PipelineStateMetal.mm
        ${POMDOG_SRC_DIR}/Graphics.Metal/RenderTarget2DMetal.hpp
        ${POMDOG_SRC_DIR}/Graphics.Metal/RenderTarget2DMetal.mm
        ${POMDOG_SRC_DIR}/Graphics.Metal/SamplerStateMetal.hpp
        ${POMDOG_SRC_DIR}/Graphics.Metal/SamplerStateMetal.mm
        ${POMDOG_SRC_DIR}/Graphics.Metal/ShaderMetal.hpp
        ${POMDOG_SRC_DIR}/Graphics.Metal/ShaderMetal.mm
        ${POMDOG_SRC_DIR}/Graphics.Metal/Texture2DMetal.hpp
        ${POMDOG_SRC_DIR}/Graphics.Metal/Texture2DMetal.mm
    >

    $<$<BOOL:${POMDOG_USE_VULKAN}>:
        # NOTE: Graphics.Vulkan
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/BufferVulkan.cpp
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/BufferVulkan.hpp
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/EffectReflectionVulkan.cpp
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/EffectReflectionVulkan.hpp
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/FormatHelper.cpp
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/FormatHelper.hpp
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/GraphicsCommandListVulkan.cpp
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/GraphicsCommandListVulkan.hpp
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/GraphicsCommandQueueVulkan.cpp
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/GraphicsCommandQueueVulkan.hpp
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/GraphicsDeviceVulkan.cpp
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/GraphicsDeviceVulkan.hpp
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/PipelineStateVulkan.cpp
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/PipelineStateVulkan.hpp
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/RenderTarget2DVulkan.cpp
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/RenderTarget2DVulkan.hpp
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/SamplerStateVulkan.cpp
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/SamplerStateVulkan.hpp
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/ShaderVulkan.cpp
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/ShaderVulkan.hpp
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/Texture2DVulkan.cpp
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/Texture2DVulkan.hpp
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/VulkanFormatHelper.cpp
        ${POMDOG_SRC_DIR}/Graphics.Vulkan/VulkanFormatHelper.hpp
    >
)
