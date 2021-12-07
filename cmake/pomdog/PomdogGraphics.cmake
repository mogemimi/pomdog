source_group(graphics                   REGULAR_EXPRESSION "pomdog/graphics/*")
source_group(graphics\\backends         REGULAR_EXPRESSION "pomdog/graphics/backends/*")
source_group(graphics\\dxgi             REGULAR_EXPRESSION "pomdog/graphics/dxgi/*")
source_group(graphics\\direct3d         REGULAR_EXPRESSION "pomdog/graphics/direct3d/*")
source_group(graphics\\direct3d11       REGULAR_EXPRESSION "pomdog/graphics/direct3d11/*")
source_group(graphics\\gl4              REGULAR_EXPRESSION "pomdog/graphics/gl4/*")
source_group(graphics\\metal            REGULAR_EXPRESSION "pomdog/graphics/metal/*")
source_group(graphics\\shader_compilers REGULAR_EXPRESSION "pomdog/graphics/shader_compilers/*")
source_group(graphics\\vulkan           REGULAR_EXPRESSION "pomdog/graphics/vulkan/*")

target_sources(pomdog_static PRIVATE
    # NOTE: graphics
    ${POMDOG_SRC_DIR}/graphics/blend_description.hpp
    ${POMDOG_SRC_DIR}/graphics/blend_factor.hpp
    ${POMDOG_SRC_DIR}/graphics/blend_operation.hpp
    ${POMDOG_SRC_DIR}/graphics/buffer_usage.hpp
    ${POMDOG_SRC_DIR}/graphics/comparison_function.hpp
    ${POMDOG_SRC_DIR}/graphics/constant_buffer.cpp
    ${POMDOG_SRC_DIR}/graphics/constant_buffer.hpp
    ${POMDOG_SRC_DIR}/graphics/cull_mode.hpp
    ${POMDOG_SRC_DIR}/graphics/depth_stencil_buffer.cpp
    ${POMDOG_SRC_DIR}/graphics/depth_stencil_buffer.hpp
    ${POMDOG_SRC_DIR}/graphics/depth_stencil_description.hpp
    ${POMDOG_SRC_DIR}/graphics/depth_stencil_operation.hpp
    ${POMDOG_SRC_DIR}/graphics/effect_annotation.hpp
    ${POMDOG_SRC_DIR}/graphics/effect_constant_description.hpp
    ${POMDOG_SRC_DIR}/graphics/effect_reflection.cpp
    ${POMDOG_SRC_DIR}/graphics/effect_reflection.hpp
    ${POMDOG_SRC_DIR}/graphics/effect_variable_class.hpp
    ${POMDOG_SRC_DIR}/graphics/effect_variable_type.hpp
    ${POMDOG_SRC_DIR}/graphics/effect_variable.hpp
    ${POMDOG_SRC_DIR}/graphics/fill_mode.hpp
    ${POMDOG_SRC_DIR}/graphics/forward_declarations.hpp
    ${POMDOG_SRC_DIR}/graphics/graphics_command_list.cpp
    ${POMDOG_SRC_DIR}/graphics/graphics_command_list.hpp
    ${POMDOG_SRC_DIR}/graphics/graphics_command_queue.cpp
    ${POMDOG_SRC_DIR}/graphics/graphics_command_queue.hpp
    ${POMDOG_SRC_DIR}/graphics/graphics_device.cpp
    ${POMDOG_SRC_DIR}/graphics/graphics_device.hpp
    ${POMDOG_SRC_DIR}/graphics/index_buffer.cpp
    ${POMDOG_SRC_DIR}/graphics/index_buffer.hpp
    ${POMDOG_SRC_DIR}/graphics/index_element_size.hpp
    ${POMDOG_SRC_DIR}/graphics/input_classification.hpp
    ${POMDOG_SRC_DIR}/graphics/input_element_format.hpp
    ${POMDOG_SRC_DIR}/graphics/input_element.hpp
    ${POMDOG_SRC_DIR}/graphics/input_layout_description.hpp
    ${POMDOG_SRC_DIR}/graphics/input_layout_helper.cpp
    ${POMDOG_SRC_DIR}/graphics/input_layout_helper.hpp
    ${POMDOG_SRC_DIR}/graphics/pipeline_state_description.hpp
    ${POMDOG_SRC_DIR}/graphics/pipeline_state.cpp
    ${POMDOG_SRC_DIR}/graphics/pipeline_state.hpp
    ${POMDOG_SRC_DIR}/graphics/presentation_parameters.hpp
    ${POMDOG_SRC_DIR}/graphics/primitive_topology.hpp
    ${POMDOG_SRC_DIR}/graphics/rasterizer_description.hpp
    ${POMDOG_SRC_DIR}/graphics/render_pass.hpp
    ${POMDOG_SRC_DIR}/graphics/render_target_blend_description.hpp
    ${POMDOG_SRC_DIR}/graphics/render_target2d.cpp
    ${POMDOG_SRC_DIR}/graphics/render_target2d.hpp
    ${POMDOG_SRC_DIR}/graphics/sampler_description.hpp
    ${POMDOG_SRC_DIR}/graphics/sampler_state.cpp
    ${POMDOG_SRC_DIR}/graphics/sampler_state.hpp
    ${POMDOG_SRC_DIR}/graphics/shader_language.hpp
    ${POMDOG_SRC_DIR}/graphics/shader_pipeline_stage.hpp
    ${POMDOG_SRC_DIR}/graphics/shader.cpp
    ${POMDOG_SRC_DIR}/graphics/shader.hpp
    ${POMDOG_SRC_DIR}/graphics/stencil_operation.hpp
    ${POMDOG_SRC_DIR}/graphics/surface_format.hpp
    ${POMDOG_SRC_DIR}/graphics/texture_address_mode.hpp
    ${POMDOG_SRC_DIR}/graphics/texture_filter.hpp
    ${POMDOG_SRC_DIR}/graphics/texture.cpp
    ${POMDOG_SRC_DIR}/graphics/texture.hpp
    ${POMDOG_SRC_DIR}/graphics/texture2d.cpp
    ${POMDOG_SRC_DIR}/graphics/texture2d.hpp
    ${POMDOG_SRC_DIR}/graphics/vertex_buffer.cpp
    ${POMDOG_SRC_DIR}/graphics/vertex_buffer.hpp
    ${POMDOG_SRC_DIR}/graphics/viewport.cpp
    ${POMDOG_SRC_DIR}/graphics/viewport.hpp

    # NOTE: graphics/shader_compilers
    ${POMDOG_SRC_DIR}/graphics/shader_compilers/glsl_compiler.cpp
    ${POMDOG_SRC_DIR}/graphics/shader_compilers/glsl_compiler.hpp
    ${POMDOG_SRC_DIR}/graphics/shader_compilers/hlsl_compiler.cpp
    ${POMDOG_SRC_DIR}/graphics/shader_compilers/hlsl_compiler.hpp
    ${POMDOG_SRC_DIR}/graphics/shader_compilers/metal_compiler.cpp
    ${POMDOG_SRC_DIR}/graphics/shader_compilers/metal_compiler.hpp

    # NOTE: graphics/backends
    ${POMDOG_SRC_DIR}/graphics/backends/buffer_bind_mode.hpp
    ${POMDOG_SRC_DIR}/graphics/backends/buffer_helper.cpp
    ${POMDOG_SRC_DIR}/graphics/backends/buffer_helper.hpp
    ${POMDOG_SRC_DIR}/graphics/backends/graphics_capabilities.hpp
    ${POMDOG_SRC_DIR}/graphics/backends/graphics_command_list_immediate.cpp
    ${POMDOG_SRC_DIR}/graphics/backends/graphics_command_list_immediate.hpp
    ${POMDOG_SRC_DIR}/graphics/backends/graphics_command_queue_immediate.cpp
    ${POMDOG_SRC_DIR}/graphics/backends/graphics_command_queue_immediate.hpp
    ${POMDOG_SRC_DIR}/graphics/backends/native_buffer.cpp
    ${POMDOG_SRC_DIR}/graphics/backends/native_buffer.hpp
    ${POMDOG_SRC_DIR}/graphics/backends/native_graphics_context.cpp
    ${POMDOG_SRC_DIR}/graphics/backends/native_graphics_context.hpp
    ${POMDOG_SRC_DIR}/graphics/backends/shader_bytecode.hpp
    ${POMDOG_SRC_DIR}/graphics/backends/shader_compile_options.hpp
    ${POMDOG_SRC_DIR}/graphics/backends/surface_format_helper.cpp
    ${POMDOG_SRC_DIR}/graphics/backends/surface_format_helper.hpp
    ${POMDOG_SRC_DIR}/graphics/backends/texture_helper.cpp
    ${POMDOG_SRC_DIR}/graphics/backends/texture_helper.hpp

    $<$<AND:$<PLATFORM_ID:Windows,Linux,Darwin>,$<BOOL:${POMDOG_USE_GL4}>>:
        # NOTE: graphics/gl4
        ${POMDOG_SRC_DIR}/graphics/gl4/blend_state_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/blend_state_gl4.hpp
        ${POMDOG_SRC_DIR}/graphics/gl4/buffer_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/buffer_gl4.hpp
        ${POMDOG_SRC_DIR}/graphics/gl4/depth_stencil_buffer_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/depth_stencil_buffer_gl4.hpp
        ${POMDOG_SRC_DIR}/graphics/gl4/depth_stencil_state_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/depth_stencil_state_gl4.hpp
        ${POMDOG_SRC_DIR}/graphics/gl4/effect_reflection_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/effect_reflection_gl4.hpp
        ${POMDOG_SRC_DIR}/graphics/gl4/error_checker.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/error_checker.hpp
        ${POMDOG_SRC_DIR}/graphics/gl4/format_helper.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/format_helper.hpp
        ${POMDOG_SRC_DIR}/graphics/gl4/graphics_context_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/graphics_context_gl4.hpp
        ${POMDOG_SRC_DIR}/graphics/gl4/graphics_device_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/graphics_device_gl4.hpp
        ${POMDOG_SRC_DIR}/graphics/gl4/input_layout_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/input_layout_gl4.hpp
        ${POMDOG_SRC_DIR}/graphics/gl4/opengl_context.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/opengl_context.hpp
        ${POMDOG_SRC_DIR}/graphics/gl4/opengl_prerequisites.hpp
        ${POMDOG_SRC_DIR}/graphics/gl4/pipeline_state_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/pipeline_state_gl4.hpp
        ${POMDOG_SRC_DIR}/graphics/gl4/rasterizer_state_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/rasterizer_state_gl4.hpp
        ${POMDOG_SRC_DIR}/graphics/gl4/render_target2d_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/render_target2d_gl4.hpp
        ${POMDOG_SRC_DIR}/graphics/gl4/sampler_state_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/sampler_state_gl4.hpp
        ${POMDOG_SRC_DIR}/graphics/gl4/shader_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/shader_gl4.hpp
        ${POMDOG_SRC_DIR}/graphics/gl4/texture2d_gl4.cpp
        ${POMDOG_SRC_DIR}/graphics/gl4/texture2d_gl4.hpp
        ${POMDOG_SRC_DIR}/graphics/gl4/typesafe_gl4.hpp
        ${POMDOG_SRC_DIR}/graphics/gl4/typesafe_helper_gl4.hpp
        ${POMDOG_SRC_DIR}/graphics/gl4/vertex_buffer_binding_gl4.hpp
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: graphics/dxgi
        ${POMDOG_SRC_DIR}/graphics/dxgi/dxgi_format_helper.cpp
        ${POMDOG_SRC_DIR}/graphics/dxgi/dxgi_format_helper.hpp
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: graphics/direct3d
        ${POMDOG_SRC_DIR}/graphics/direct3d/hlsl_compiling.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d/hlsl_compiling.hpp
        ${POMDOG_SRC_DIR}/graphics/direct3d/hlsl_reflection_helper.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d/hlsl_reflection_helper.hpp
        ${POMDOG_SRC_DIR}/graphics/direct3d/prerequisites_direct3d.hpp
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: graphics/direct3d11
        ${POMDOG_SRC_DIR}/graphics/direct3d11/buffer_direct3d11.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/buffer_direct3d11.hpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/depth_stencil_buffer_direct3d11.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/depth_stencil_buffer_direct3d11.hpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/effect_reflection_direct3d11.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/effect_reflection_direct3d11.hpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/format_helper.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/format_helper.hpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/graphics_context_direct3d11.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/graphics_context_direct3d11.hpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/graphics_device_direct3d11.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/graphics_device_direct3d11.hpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/pipeline_state_direct3d11.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/pipeline_state_direct3d11.hpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/prerequisites_direct3d11.hpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/render_target2d_direct3d11.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/render_target2d_direct3d11.hpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/sampler_state_direct3d11.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/sampler_state_direct3d11.hpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/shader_direct3d11.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/shader_direct3d11.hpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/texture2d_direct3d11.cpp
        ${POMDOG_SRC_DIR}/graphics/direct3d11/texture2d_direct3d11.hpp
    >

    $<$<PLATFORM_ID:Darwin>:
        # NOTE: graphics/metal
        ${POMDOG_SRC_DIR}/graphics/metal/buffer_metal.hpp
        ${POMDOG_SRC_DIR}/graphics/metal/buffer_metal.mm
        ${POMDOG_SRC_DIR}/graphics/metal/constants_metal.hpp
        ${POMDOG_SRC_DIR}/graphics/metal/depth_stencil_buffer_metal.hpp
        ${POMDOG_SRC_DIR}/graphics/metal/depth_stencil_buffer_metal.mm
        ${POMDOG_SRC_DIR}/graphics/metal/effect_reflection_metal.hpp
        ${POMDOG_SRC_DIR}/graphics/metal/effect_reflection_metal.mm
        ${POMDOG_SRC_DIR}/graphics/metal/graphics_context_metal.hpp
        ${POMDOG_SRC_DIR}/graphics/metal/graphics_context_metal.mm
        ${POMDOG_SRC_DIR}/graphics/metal/graphics_device_metal.hpp
        ${POMDOG_SRC_DIR}/graphics/metal/graphics_device_metal.mm
        ${POMDOG_SRC_DIR}/graphics/metal/metal_format_helper.hpp
        ${POMDOG_SRC_DIR}/graphics/metal/metal_format_helper.mm
        ${POMDOG_SRC_DIR}/graphics/metal/pipeline_state_metal.hpp
        ${POMDOG_SRC_DIR}/graphics/metal/pipeline_state_metal.mm
        ${POMDOG_SRC_DIR}/graphics/metal/render_target2d_metal.hpp
        ${POMDOG_SRC_DIR}/graphics/metal/render_target2d_metal.mm
        ${POMDOG_SRC_DIR}/graphics/metal/sampler_state_metal.hpp
        ${POMDOG_SRC_DIR}/graphics/metal/sampler_state_metal.mm
        ${POMDOG_SRC_DIR}/graphics/metal/shader_metal.hpp
        ${POMDOG_SRC_DIR}/graphics/metal/shader_metal.mm
        ${POMDOG_SRC_DIR}/graphics/metal/texture2d_metal.hpp
        ${POMDOG_SRC_DIR}/graphics/metal/texture2d_metal.mm
    >

    $<$<BOOL:${POMDOG_USE_VULKAN}>:
        # NOTE: graphics/vulkan
        ${POMDOG_SRC_DIR}/graphics/vulkan/buffer_vulkan.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/buffer_vulkan.hpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/effect_reflection_vulkan.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/effect_reflection_vulkan.hpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/format_helper.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/format_helper.hpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/graphics_command_list_vulkan.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/graphics_command_list_vulkan.hpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/graphics_command_queue_vulkan.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/graphics_command_queue_vulkan.hpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/graphics_device_vulkan.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/graphics_device_vulkan.hpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/pipeline_state_vulkan.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/pipeline_state_vulkan.hpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/render_target2d_vulkan.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/render_target2d_vulkan.hpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/sampler_state_vulkan.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/sampler_state_vulkan.hpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/shader_vulkan.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/shader_vulkan.hpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/texture2d_vulkan.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/texture2d_vulkan.hpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/vulkan_format_helper.cpp
        ${POMDOG_SRC_DIR}/graphics/vulkan/vulkan_format_helper.hpp
    >
)
