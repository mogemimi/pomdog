source_group(graphics                   REGULAR_EXPRESSION "pomdog/graphics/*")
source_group(graphics/backends          REGULAR_EXPRESSION "pomdog/graphics/backends/*")
source_group(graphics/dxgi              REGULAR_EXPRESSION "pomdog/graphics/dxgi/*")
source_group(graphics/direct3d          REGULAR_EXPRESSION "pomdog/graphics/direct3d/*")
source_group(graphics/direct3d11        REGULAR_EXPRESSION "pomdog/graphics/direct3d11/*")
source_group(graphics/gl4               REGULAR_EXPRESSION "pomdog/graphics/gl4/*")
source_group(graphics/metal             REGULAR_EXPRESSION "pomdog/graphics/metal/*")
source_group(graphics/shader_compilers  REGULAR_EXPRESSION "pomdog/graphics/shader_compilers/*")
source_group(graphics/vulkan            REGULAR_EXPRESSION "pomdog/graphics/vulkan/*")

target_sources(pomdog_static PRIVATE
    # NOTE: graphics
    ${POMDOG_SRC_DIR}/graphics/blend_description.h
    ${POMDOG_SRC_DIR}/graphics/blend_factor.h
    ${POMDOG_SRC_DIR}/graphics/blend_operation.h
    ${POMDOG_SRC_DIR}/graphics/buffer_usage.h
    ${POMDOG_SRC_DIR}/graphics/comparison_function.h
    ${POMDOG_SRC_DIR}/graphics/constant_buffer.cpp
    ${POMDOG_SRC_DIR}/graphics/constant_buffer.h
    ${POMDOG_SRC_DIR}/graphics/cull_mode.h
    ${POMDOG_SRC_DIR}/graphics/depth_stencil_buffer.cpp
    ${POMDOG_SRC_DIR}/graphics/depth_stencil_buffer.h
    ${POMDOG_SRC_DIR}/graphics/depth_stencil_description.h
    ${POMDOG_SRC_DIR}/graphics/depth_stencil_operation.h
    ${POMDOG_SRC_DIR}/graphics/effect_annotation.h
    ${POMDOG_SRC_DIR}/graphics/effect_constant_description.h
    ${POMDOG_SRC_DIR}/graphics/effect_reflection.cpp
    ${POMDOG_SRC_DIR}/graphics/effect_reflection.h
    ${POMDOG_SRC_DIR}/graphics/effect_variable_class.h
    ${POMDOG_SRC_DIR}/graphics/effect_variable_type.h
    ${POMDOG_SRC_DIR}/graphics/effect_variable.h
    ${POMDOG_SRC_DIR}/graphics/fill_mode.h
    ${POMDOG_SRC_DIR}/graphics/forward_declarations.h
    ${POMDOG_SRC_DIR}/graphics/graphics_command_list.cpp
    ${POMDOG_SRC_DIR}/graphics/graphics_command_list.h
    ${POMDOG_SRC_DIR}/graphics/graphics_command_queue.cpp
    ${POMDOG_SRC_DIR}/graphics/graphics_command_queue.h
    ${POMDOG_SRC_DIR}/graphics/graphics_device.cpp
    ${POMDOG_SRC_DIR}/graphics/graphics_device.h
    ${POMDOG_SRC_DIR}/graphics/index_buffer.cpp
    ${POMDOG_SRC_DIR}/graphics/index_buffer.h
    ${POMDOG_SRC_DIR}/graphics/index_element_size.h
    ${POMDOG_SRC_DIR}/graphics/input_classification.h
    ${POMDOG_SRC_DIR}/graphics/input_element_format.h
    ${POMDOG_SRC_DIR}/graphics/input_element.h
    ${POMDOG_SRC_DIR}/graphics/input_layout_description.h
    ${POMDOG_SRC_DIR}/graphics/input_layout_helper.cpp
    ${POMDOG_SRC_DIR}/graphics/input_layout_helper.h
    ${POMDOG_SRC_DIR}/graphics/pipeline_state_description.h
    ${POMDOG_SRC_DIR}/graphics/pipeline_state.cpp
    ${POMDOG_SRC_DIR}/graphics/pipeline_state.h
    ${POMDOG_SRC_DIR}/graphics/presentation_parameters.h
    ${POMDOG_SRC_DIR}/graphics/primitive_topology.h
    ${POMDOG_SRC_DIR}/graphics/rasterizer_description.h
    ${POMDOG_SRC_DIR}/graphics/render_pass.h
    ${POMDOG_SRC_DIR}/graphics/render_target_blend_description.h
    ${POMDOG_SRC_DIR}/graphics/render_target2d.cpp
    ${POMDOG_SRC_DIR}/graphics/render_target2d.h
    ${POMDOG_SRC_DIR}/graphics/sampler_description.h
    ${POMDOG_SRC_DIR}/graphics/sampler_state.cpp
    ${POMDOG_SRC_DIR}/graphics/sampler_state.h
    ${POMDOG_SRC_DIR}/graphics/shader_language.h
    ${POMDOG_SRC_DIR}/graphics/shader_pipeline_stage.h
    ${POMDOG_SRC_DIR}/graphics/shader.cpp
    ${POMDOG_SRC_DIR}/graphics/shader.h
    ${POMDOG_SRC_DIR}/graphics/stencil_operation.h
    ${POMDOG_SRC_DIR}/graphics/surface_format.h
    ${POMDOG_SRC_DIR}/graphics/texture_address_mode.h
    ${POMDOG_SRC_DIR}/graphics/texture_filter.h
    ${POMDOG_SRC_DIR}/graphics/texture.cpp
    ${POMDOG_SRC_DIR}/graphics/texture.h
    ${POMDOG_SRC_DIR}/graphics/texture2d.cpp
    ${POMDOG_SRC_DIR}/graphics/texture2d.h
    ${POMDOG_SRC_DIR}/graphics/vertex_buffer.cpp
    ${POMDOG_SRC_DIR}/graphics/vertex_buffer.h
    ${POMDOG_SRC_DIR}/graphics/viewport.cpp
    ${POMDOG_SRC_DIR}/graphics/viewport.h

    # NOTE: graphics/shader_compilers
    ${POMDOG_SRC_DIR}/graphics/shader_compilers/glsl_compiler.cpp
    ${POMDOG_SRC_DIR}/graphics/shader_compilers/glsl_compiler.h
    ${POMDOG_SRC_DIR}/graphics/shader_compilers/hlsl_compiler.cpp
    ${POMDOG_SRC_DIR}/graphics/shader_compilers/hlsl_compiler.h
    ${POMDOG_SRC_DIR}/graphics/shader_compilers/metal_compiler.cpp
    ${POMDOG_SRC_DIR}/graphics/shader_compilers/metal_compiler.h

    # NOTE: graphics/backends
    ${POMDOG_SRC_DIR}/graphics/backends/buffer_bind_mode.h
    ${POMDOG_SRC_DIR}/graphics/backends/buffer_helper.cpp
    ${POMDOG_SRC_DIR}/graphics/backends/buffer_helper.h
    ${POMDOG_SRC_DIR}/graphics/backends/graphics_capabilities.h
    ${POMDOG_SRC_DIR}/graphics/backends/graphics_command_list_immediate.cpp
    ${POMDOG_SRC_DIR}/graphics/backends/graphics_command_list_immediate.h
    ${POMDOG_SRC_DIR}/graphics/backends/graphics_command_queue_immediate.cpp
    ${POMDOG_SRC_DIR}/graphics/backends/graphics_command_queue_immediate.h
    ${POMDOG_SRC_DIR}/graphics/backends/native_buffer.cpp
    ${POMDOG_SRC_DIR}/graphics/backends/native_buffer.h
    ${POMDOG_SRC_DIR}/graphics/backends/native_graphics_context.cpp
    ${POMDOG_SRC_DIR}/graphics/backends/native_graphics_context.h
    ${POMDOG_SRC_DIR}/graphics/backends/shader_bytecode.h
    ${POMDOG_SRC_DIR}/graphics/backends/shader_compile_options.h
    ${POMDOG_SRC_DIR}/graphics/backends/surface_format_helper.cpp
    ${POMDOG_SRC_DIR}/graphics/backends/surface_format_helper.h
    ${POMDOG_SRC_DIR}/graphics/backends/texture_helper.cpp
    ${POMDOG_SRC_DIR}/graphics/backends/texture_helper.h

    $<$<AND:$<PLATFORM_ID:Windows,Linux,Darwin>,$<BOOL:${POMDOG_USE_GL4}>>:
        # NOTE: graphics/gl4
        ${POMDOG_SRC_DIR}/graphics/gl4/blend_state_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/blend_state_gl4.h
        ${POMDOG_SRC_DIR}/graphics/gl4/buffer_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/buffer_gl4.h
        ${POMDOG_SRC_DIR}/graphics/gl4/depth_stencil_buffer_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/depth_stencil_buffer_gl4.h
        ${POMDOG_SRC_DIR}/graphics/gl4/depth_stencil_state_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/depth_stencil_state_gl4.h
        ${POMDOG_SRC_DIR}/graphics/gl4/effect_reflection_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/effect_reflection_gl4.h
        ${POMDOG_SRC_DIR}/graphics/gl4/error_checker.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/error_checker.h
        ${POMDOG_SRC_DIR}/graphics/gl4/format_helper.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/format_helper.h
        ${POMDOG_SRC_DIR}/graphics/gl4/graphics_context_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/graphics_context_gl4.h
        ${POMDOG_SRC_DIR}/graphics/gl4/graphics_device_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/graphics_device_gl4.h
        ${POMDOG_SRC_DIR}/graphics/gl4/input_layout_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/input_layout_gl4.h
        ${POMDOG_SRC_DIR}/graphics/gl4/opengl_context.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/opengl_context.h
        ${POMDOG_SRC_DIR}/graphics/gl4/opengl_prerequisites.h
        ${POMDOG_SRC_DIR}/graphics/gl4/pipeline_state_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/pipeline_state_gl4.h
        ${POMDOG_SRC_DIR}/graphics/gl4/rasterizer_state_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/rasterizer_state_gl4.h
        ${POMDOG_SRC_DIR}/graphics/gl4/render_target2d_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/render_target2d_gl4.h
        ${POMDOG_SRC_DIR}/graphics/gl4/sampler_state_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/sampler_state_gl4.h
        ${POMDOG_SRC_DIR}/graphics/gl4/shader_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/shader_gl4.h
        ${POMDOG_SRC_DIR}/graphics/gl4/texture2d_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/texture2d_gl4.h
        ${POMDOG_SRC_DIR}/graphics/gl4/typesafe_gl4.h
        ${POMDOG_SRC_DIR}/graphics/gl4/typesafe_helper_gl4.h
        ${POMDOG_SRC_DIR}/graphics/gl4/vertex_buffer_binding_gl4.h
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: graphics/dxgi
        ${POMDOG_SRC_DIR}/graphics/dxgi/dxgi_format_helper.cpp
        ${POMDOG_SRC_DIR}/graphics/dxgi/dxgi_format_helper.h
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: graphics/direct3d
        ${POMDOG_SRC_DIR}/graphics/direct3d/hlsl_compiling.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d/hlsl_compiling.h
        ${POMDOG_SRC_DIR}/graphics/direct3d/hlsl_reflection_helper.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d/hlsl_reflection_helper.h
        ${POMDOG_SRC_DIR}/graphics/direct3d/prerequisites_direct3d.h
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: graphics/direct3d11
        ${POMDOG_SRC_DIR}/graphics/direct3d11/buffer_direct3d11.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/buffer_direct3d11.h
        ${POMDOG_SRC_DIR}/graphics/direct3d11/depth_stencil_buffer_direct3d11.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/depth_stencil_buffer_direct3d11.h
        ${POMDOG_SRC_DIR}/graphics/direct3d11/effect_reflection_direct3d11.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/effect_reflection_direct3d11.h
        ${POMDOG_SRC_DIR}/graphics/direct3d11/format_helper.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/format_helper.h
        ${POMDOG_SRC_DIR}/graphics/direct3d11/graphics_context_direct3d11.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/graphics_context_direct3d11.h
        ${POMDOG_SRC_DIR}/graphics/direct3d11/graphics_device_direct3d11.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/graphics_device_direct3d11.h
        ${POMDOG_SRC_DIR}/graphics/direct3d11/pipeline_state_direct3d11.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/pipeline_state_direct3d11.h
        ${POMDOG_SRC_DIR}/graphics/direct3d11/prerequisites_direct3d11.h
        ${POMDOG_SRC_DIR}/graphics/direct3d11/render_target2d_direct3d11.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/render_target2d_direct3d11.h
        ${POMDOG_SRC_DIR}/graphics/direct3d11/sampler_state_direct3d11.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/sampler_state_direct3d11.h
        ${POMDOG_SRC_DIR}/graphics/direct3d11/shader_direct3d11.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/shader_direct3d11.h
        ${POMDOG_SRC_DIR}/graphics/direct3d11/texture2d_direct3d11.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/texture2d_direct3d11.h
    >

    $<$<PLATFORM_ID:Darwin>:
        # NOTE: graphics/metal
        ${POMDOG_SRC_DIR}/graphics/metal/buffer_metal.h
        ${POMDOG_SRC_DIR}/graphics/metal/buffer_metal.mm
        ${POMDOG_SRC_DIR}/graphics/metal/constants_metal.h
        ${POMDOG_SRC_DIR}/graphics/metal/depth_stencil_buffer_metal.h
        ${POMDOG_SRC_DIR}/graphics/metal/depth_stencil_buffer_metal.mm
        ${POMDOG_SRC_DIR}/graphics/metal/effect_reflection_metal.h
        ${POMDOG_SRC_DIR}/graphics/metal/effect_reflection_metal.mm
        ${POMDOG_SRC_DIR}/graphics/metal/graphics_context_metal.h
        ${POMDOG_SRC_DIR}/graphics/metal/graphics_context_metal.mm
        ${POMDOG_SRC_DIR}/graphics/metal/graphics_device_metal.h
        ${POMDOG_SRC_DIR}/graphics/metal/graphics_device_metal.mm
        ${POMDOG_SRC_DIR}/graphics/metal/metal_format_helper.h
        ${POMDOG_SRC_DIR}/graphics/metal/metal_format_helper.mm
        ${POMDOG_SRC_DIR}/graphics/metal/pipeline_state_metal.h
        ${POMDOG_SRC_DIR}/graphics/metal/pipeline_state_metal.mm
        ${POMDOG_SRC_DIR}/graphics/metal/render_target2d_metal.h
        ${POMDOG_SRC_DIR}/graphics/metal/render_target2d_metal.mm
        ${POMDOG_SRC_DIR}/graphics/metal/sampler_state_metal.h
        ${POMDOG_SRC_DIR}/graphics/metal/sampler_state_metal.mm
        ${POMDOG_SRC_DIR}/graphics/metal/shader_metal.h
        ${POMDOG_SRC_DIR}/graphics/metal/shader_metal.mm
        ${POMDOG_SRC_DIR}/graphics/metal/texture2d_metal.h
        ${POMDOG_SRC_DIR}/graphics/metal/texture2d_metal.mm
    >

    $<$<BOOL:${POMDOG_USE_VULKAN}>:
        # NOTE: graphics/vulkan
        ${POMDOG_SRC_DIR}/graphics/vulkan/buffer_vulkan.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/buffer_vulkan.h
        ${POMDOG_SRC_DIR}/graphics/vulkan/effect_reflection_vulkan.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/effect_reflection_vulkan.h
        ${POMDOG_SRC_DIR}/graphics/vulkan/format_helper.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/format_helper.h
        ${POMDOG_SRC_DIR}/graphics/vulkan/graphics_command_list_vulkan.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/graphics_command_list_vulkan.h
        ${POMDOG_SRC_DIR}/graphics/vulkan/graphics_command_queue_vulkan.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/graphics_command_queue_vulkan.h
        ${POMDOG_SRC_DIR}/graphics/vulkan/graphics_device_vulkan.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/graphics_device_vulkan.h
        ${POMDOG_SRC_DIR}/graphics/vulkan/pipeline_state_vulkan.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/pipeline_state_vulkan.h
        ${POMDOG_SRC_DIR}/graphics/vulkan/render_target2d_vulkan.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/render_target2d_vulkan.h
        ${POMDOG_SRC_DIR}/graphics/vulkan/sampler_state_vulkan.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/sampler_state_vulkan.h
        ${POMDOG_SRC_DIR}/graphics/vulkan/shader_vulkan.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/shader_vulkan.h
        ${POMDOG_SRC_DIR}/graphics/vulkan/texture2d_vulkan.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/texture2d_vulkan.h
        ${POMDOG_SRC_DIR}/graphics/vulkan/vulkan_format_helper.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/vulkan_format_helper.h
    >
)
