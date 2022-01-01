source_group(gpu                    REGULAR_EXPRESSION "pomdog/gpu/*")
source_group(gpu/backends           REGULAR_EXPRESSION "pomdog/gpu/backends/*")
source_group(gpu/dxgi               REGULAR_EXPRESSION "pomdog/gpu/dxgi/*")
source_group(gpu/direct3d           REGULAR_EXPRESSION "pomdog/gpu/direct3d/*")
source_group(gpu/direct3d11         REGULAR_EXPRESSION "pomdog/gpu/direct3d11/*")
source_group(gpu/gl4                REGULAR_EXPRESSION "pomdog/gpu/gl4/*")
source_group(gpu/metal              REGULAR_EXPRESSION "pomdog/gpu/metal/*")
source_group(gpu/shader_compilers   REGULAR_EXPRESSION "pomdog/gpu/shader_compilers/*")
source_group(gpu/shader_reflections REGULAR_EXPRESSION "pomdog/gpu/shader_reflections/*")
source_group(gpu/vulkan             REGULAR_EXPRESSION "pomdog/gpu/vulkan/*")

target_sources(pomdog_static PRIVATE
    # NOTE: graphics
    ${POMDOG_SRC_DIR}/gpu/blend_descriptor.cpp
    ${POMDOG_SRC_DIR}/gpu/blend_descriptor.h
    ${POMDOG_SRC_DIR}/gpu/blend_factor.h
    ${POMDOG_SRC_DIR}/gpu/blend_operation.h
    ${POMDOG_SRC_DIR}/gpu/buffer_usage.h
    ${POMDOG_SRC_DIR}/gpu/buffer.cpp
    ${POMDOG_SRC_DIR}/gpu/buffer.h
    ${POMDOG_SRC_DIR}/gpu/command_list.cpp
    ${POMDOG_SRC_DIR}/gpu/command_list.h
    ${POMDOG_SRC_DIR}/gpu/command_queue.cpp
    ${POMDOG_SRC_DIR}/gpu/command_queue.h
    ${POMDOG_SRC_DIR}/gpu/comparison_function.h
    ${POMDOG_SRC_DIR}/gpu/constant_buffer.cpp
    ${POMDOG_SRC_DIR}/gpu/constant_buffer.h
    ${POMDOG_SRC_DIR}/gpu/cull_mode.h
    ${POMDOG_SRC_DIR}/gpu/depth_stencil_buffer.cpp
    ${POMDOG_SRC_DIR}/gpu/depth_stencil_buffer.h
    ${POMDOG_SRC_DIR}/gpu/depth_stencil_descriptor.cpp
    ${POMDOG_SRC_DIR}/gpu/depth_stencil_descriptor.h
    ${POMDOG_SRC_DIR}/gpu/depth_stencil_operation.h
    ${POMDOG_SRC_DIR}/gpu/fill_mode.h
    ${POMDOG_SRC_DIR}/gpu/forward_declarations.h
    ${POMDOG_SRC_DIR}/gpu/graphics_device.cpp
    ${POMDOG_SRC_DIR}/gpu/graphics_device.h
    ${POMDOG_SRC_DIR}/gpu/index_buffer.cpp
    ${POMDOG_SRC_DIR}/gpu/index_buffer.h
    ${POMDOG_SRC_DIR}/gpu/index_format.h
    ${POMDOG_SRC_DIR}/gpu/input_classification.h
    ${POMDOG_SRC_DIR}/gpu/input_element_format.h
    ${POMDOG_SRC_DIR}/gpu/input_element.h
    ${POMDOG_SRC_DIR}/gpu/input_layout_descriptor.h
    ${POMDOG_SRC_DIR}/gpu/input_layout_helper.cpp
    ${POMDOG_SRC_DIR}/gpu/input_layout_helper.h
    ${POMDOG_SRC_DIR}/gpu/pipeline_descriptor.h
    ${POMDOG_SRC_DIR}/gpu/pipeline_state.cpp
    ${POMDOG_SRC_DIR}/gpu/pipeline_state.h
    ${POMDOG_SRC_DIR}/gpu/pixel_format.h
    ${POMDOG_SRC_DIR}/gpu/presentation_parameters.h
    ${POMDOG_SRC_DIR}/gpu/primitive_topology.h
    ${POMDOG_SRC_DIR}/gpu/rasterizer_descriptor.cpp
    ${POMDOG_SRC_DIR}/gpu/rasterizer_descriptor.h
    ${POMDOG_SRC_DIR}/gpu/render_pass.h
    ${POMDOG_SRC_DIR}/gpu/render_target_blend_descriptor.cpp
    ${POMDOG_SRC_DIR}/gpu/render_target_blend_descriptor.h
    ${POMDOG_SRC_DIR}/gpu/render_target2d.cpp
    ${POMDOG_SRC_DIR}/gpu/render_target2d.h
    ${POMDOG_SRC_DIR}/gpu/sampler_descriptor.cpp
    ${POMDOG_SRC_DIR}/gpu/sampler_descriptor.h
    ${POMDOG_SRC_DIR}/gpu/sampler_state.cpp
    ${POMDOG_SRC_DIR}/gpu/sampler_state.h
    ${POMDOG_SRC_DIR}/gpu/shader_language.h
    ${POMDOG_SRC_DIR}/gpu/shader_pipeline_stage.h
    ${POMDOG_SRC_DIR}/gpu/shader.cpp
    ${POMDOG_SRC_DIR}/gpu/shader.h
    ${POMDOG_SRC_DIR}/gpu/stencil_operation.h
    ${POMDOG_SRC_DIR}/gpu/texture_address_mode.h
    ${POMDOG_SRC_DIR}/gpu/texture_filter.h
    ${POMDOG_SRC_DIR}/gpu/texture.cpp
    ${POMDOG_SRC_DIR}/gpu/texture.h
    ${POMDOG_SRC_DIR}/gpu/texture2d.cpp
    ${POMDOG_SRC_DIR}/gpu/texture2d.h
    ${POMDOG_SRC_DIR}/gpu/vertex_buffer.cpp
    ${POMDOG_SRC_DIR}/gpu/vertex_buffer.h
    ${POMDOG_SRC_DIR}/gpu/viewport.cpp
    ${POMDOG_SRC_DIR}/gpu/viewport.h

    # NOTE: gpu/shader_compilers
    ${POMDOG_SRC_DIR}/gpu/shader_compilers/glsl_compiler.cpp
    ${POMDOG_SRC_DIR}/gpu/shader_compilers/glsl_compiler.h
    ${POMDOG_SRC_DIR}/gpu/shader_compilers/hlsl_compiler.cpp
    ${POMDOG_SRC_DIR}/gpu/shader_compilers/hlsl_compiler.h
    ${POMDOG_SRC_DIR}/gpu/shader_compilers/metal_compiler.cpp
    ${POMDOG_SRC_DIR}/gpu/shader_compilers/metal_compiler.h

    # NOTE: gpu/shader_reflections
    ${POMDOG_SRC_DIR}/gpu/shader_reflections/effect_annotation.h
    ${POMDOG_SRC_DIR}/gpu/shader_reflections/effect_constant_description.h
    ${POMDOG_SRC_DIR}/gpu/shader_reflections/effect_reflection.cpp
    ${POMDOG_SRC_DIR}/gpu/shader_reflections/effect_reflection.h
    ${POMDOG_SRC_DIR}/gpu/shader_reflections/effect_variable_class.h
    ${POMDOG_SRC_DIR}/gpu/shader_reflections/effect_variable_type.h
    ${POMDOG_SRC_DIR}/gpu/shader_reflections/effect_variable.h

    # NOTE: gpu/backends
    ${POMDOG_SRC_DIR}/gpu/backends/buffer_bind_mode.h
    ${POMDOG_SRC_DIR}/gpu/backends/buffer_helper.cpp
    ${POMDOG_SRC_DIR}/gpu/backends/buffer_helper.h
    ${POMDOG_SRC_DIR}/gpu/backends/command_list_immediate.cpp
    ${POMDOG_SRC_DIR}/gpu/backends/command_list_immediate.h
    ${POMDOG_SRC_DIR}/gpu/backends/command_queue_immediate.cpp
    ${POMDOG_SRC_DIR}/gpu/backends/command_queue_immediate.h
    ${POMDOG_SRC_DIR}/gpu/backends/graphics_capabilities.h
    ${POMDOG_SRC_DIR}/gpu/backends/graphics_context.cpp
    ${POMDOG_SRC_DIR}/gpu/backends/graphics_context.h
    ${POMDOG_SRC_DIR}/gpu/backends/shader_bytecode.h
    ${POMDOG_SRC_DIR}/gpu/backends/shader_compile_options.h
    ${POMDOG_SRC_DIR}/gpu/backends/surface_format_helper.cpp
    ${POMDOG_SRC_DIR}/gpu/backends/surface_format_helper.h
    ${POMDOG_SRC_DIR}/gpu/backends/texture_helper.cpp
    ${POMDOG_SRC_DIR}/gpu/backends/texture_helper.h

    $<$<AND:$<PLATFORM_ID:Windows,Linux,Darwin>,$<BOOL:${POMDOG_USE_GL4}>>:
        # NOTE: gpu/gl4
        ${POMDOG_SRC_DIR}/gpu/gl4/blend_state_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/blend_state_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/buffer_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/buffer_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/depth_stencil_buffer_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/depth_stencil_buffer_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/depth_stencil_state_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/depth_stencil_state_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/effect_reflection_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/effect_reflection_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/error_checker.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/error_checker.h
        ${POMDOG_SRC_DIR}/gpu/gl4/format_helper.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/format_helper.h
        ${POMDOG_SRC_DIR}/gpu/gl4/graphics_context_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/graphics_context_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/graphics_device_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/graphics_device_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/input_layout_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/input_layout_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/opengl_context.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/opengl_context.h
        ${POMDOG_SRC_DIR}/gpu/gl4/opengl_prerequisites.h
        ${POMDOG_SRC_DIR}/gpu/gl4/pipeline_state_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/pipeline_state_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/rasterizer_state_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/rasterizer_state_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/render_target2d_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/render_target2d_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/sampler_state_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/sampler_state_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/shader_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/shader_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/texture2d_gl4.cpp
        ${POMDOG_SRC_DIR}/gpu/gl4/texture2d_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/typesafe_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/typesafe_helper_gl4.h
        ${POMDOG_SRC_DIR}/gpu/gl4/vertex_buffer_binding_gl4.h
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: gpu/dxgi
        ${POMDOG_SRC_DIR}/gpu/dxgi/dxgi_format_helper.cpp
        ${POMDOG_SRC_DIR}/gpu/dxgi/dxgi_format_helper.h
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: gpu/direct3d
        ${POMDOG_SRC_DIR}/gpu/direct3d/hlsl_compiling.cpp
        ${POMDOG_SRC_DIR}/gpu/direct3d/hlsl_compiling.h
        ${POMDOG_SRC_DIR}/gpu/direct3d/hlsl_reflection_helper.cpp
        ${POMDOG_SRC_DIR}/gpu/direct3d/hlsl_reflection_helper.h
        ${POMDOG_SRC_DIR}/gpu/direct3d/prerequisites_direct3d.h
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: gpu/direct3d11
        ${POMDOG_SRC_DIR}/gpu/direct3d11/buffer_direct3d11.cpp
        ${POMDOG_SRC_DIR}/gpu/direct3d11/buffer_direct3d11.h
        ${POMDOG_SRC_DIR}/gpu/direct3d11/depth_stencil_buffer_direct3d11.cpp
        ${POMDOG_SRC_DIR}/gpu/direct3d11/depth_stencil_buffer_direct3d11.h
        ${POMDOG_SRC_DIR}/gpu/direct3d11/effect_reflection_direct3d11.cpp
        ${POMDOG_SRC_DIR}/gpu/direct3d11/effect_reflection_direct3d11.h
        ${POMDOG_SRC_DIR}/gpu/direct3d11/format_helper.cpp
        ${POMDOG_SRC_DIR}/gpu/direct3d11/format_helper.h
        ${POMDOG_SRC_DIR}/gpu/direct3d11/graphics_context_direct3d11.cpp
        ${POMDOG_SRC_DIR}/gpu/direct3d11/graphics_context_direct3d11.h
        ${POMDOG_SRC_DIR}/gpu/direct3d11/graphics_device_direct3d11.cpp
        ${POMDOG_SRC_DIR}/gpu/direct3d11/graphics_device_direct3d11.h
        ${POMDOG_SRC_DIR}/gpu/direct3d11/pipeline_state_direct3d11.cpp
        ${POMDOG_SRC_DIR}/gpu/direct3d11/pipeline_state_direct3d11.h
        ${POMDOG_SRC_DIR}/gpu/direct3d11/prerequisites_direct3d11.h
        ${POMDOG_SRC_DIR}/gpu/direct3d11/render_target2d_direct3d11.cpp
        ${POMDOG_SRC_DIR}/gpu/direct3d11/render_target2d_direct3d11.h
        ${POMDOG_SRC_DIR}/gpu/direct3d11/sampler_state_direct3d11.cpp
        ${POMDOG_SRC_DIR}/gpu/direct3d11/sampler_state_direct3d11.h
        ${POMDOG_SRC_DIR}/gpu/direct3d11/shader_direct3d11.cpp
        ${POMDOG_SRC_DIR}/gpu/direct3d11/shader_direct3d11.h
        ${POMDOG_SRC_DIR}/gpu/direct3d11/texture2d_direct3d11.cpp
        ${POMDOG_SRC_DIR}/gpu/direct3d11/texture2d_direct3d11.h
    >

    $<$<PLATFORM_ID:Darwin>:
        # NOTE: gpu/metal
        ${POMDOG_SRC_DIR}/gpu/metal/buffer_metal.h
        ${POMDOG_SRC_DIR}/gpu/metal/buffer_metal.mm
        ${POMDOG_SRC_DIR}/gpu/metal/constants_metal.h
        ${POMDOG_SRC_DIR}/gpu/metal/depth_stencil_buffer_metal.h
        ${POMDOG_SRC_DIR}/gpu/metal/depth_stencil_buffer_metal.mm
        ${POMDOG_SRC_DIR}/gpu/metal/effect_reflection_metal.h
        ${POMDOG_SRC_DIR}/gpu/metal/effect_reflection_metal.mm
        ${POMDOG_SRC_DIR}/gpu/metal/graphics_context_metal.h
        ${POMDOG_SRC_DIR}/gpu/metal/graphics_context_metal.mm
        ${POMDOG_SRC_DIR}/gpu/metal/graphics_device_metal.h
        ${POMDOG_SRC_DIR}/gpu/metal/graphics_device_metal.mm
        ${POMDOG_SRC_DIR}/gpu/metal/metal_format_helper.h
        ${POMDOG_SRC_DIR}/gpu/metal/metal_format_helper.mm
        ${POMDOG_SRC_DIR}/gpu/metal/pipeline_state_metal.h
        ${POMDOG_SRC_DIR}/gpu/metal/pipeline_state_metal.mm
        ${POMDOG_SRC_DIR}/gpu/metal/render_target2d_metal.h
        ${POMDOG_SRC_DIR}/gpu/metal/render_target2d_metal.mm
        ${POMDOG_SRC_DIR}/gpu/metal/sampler_state_metal.h
        ${POMDOG_SRC_DIR}/gpu/metal/sampler_state_metal.mm
        ${POMDOG_SRC_DIR}/gpu/metal/shader_metal.h
        ${POMDOG_SRC_DIR}/gpu/metal/shader_metal.mm
        ${POMDOG_SRC_DIR}/gpu/metal/texture2d_metal.h
        ${POMDOG_SRC_DIR}/gpu/metal/texture2d_metal.mm
    >

    $<$<BOOL:${POMDOG_USE_VULKAN}>:
        # NOTE: gpu/vulkan
        ${POMDOG_SRC_DIR}/gpu/vulkan/buffer_vulkan.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/buffer_vulkan.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/command_list_vulkan.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/command_list_vulkan.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/command_queue_vulkan.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/command_queue_vulkan.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/effect_reflection_vulkan.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/effect_reflection_vulkan.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/format_helper.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/format_helper.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/graphics_device_vulkan.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/graphics_device_vulkan.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/pipeline_state_vulkan.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/pipeline_state_vulkan.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/render_target2d_vulkan.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/render_target2d_vulkan.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/sampler_state_vulkan.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/sampler_state_vulkan.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/shader_vulkan.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/shader_vulkan.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/texture2d_vulkan.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/texture2d_vulkan.h
        ${POMDOG_SRC_DIR}/gpu/vulkan/vulkan_format_helper.cpp
        ${POMDOG_SRC_DIR}/gpu/vulkan/vulkan_format_helper.h
    >
)
