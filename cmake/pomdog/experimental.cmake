source_group(experimental\\async                        REGULAR_EXPRESSION "pomdog/experimental/async/*")
source_group(experimental\\ecs                          REGULAR_EXPRESSION "pomdog/experimental/ecs/*")
source_group(experimental\\gltf                         REGULAR_EXPRESSION "pomdog/experimental/gltf/*")
source_group(experimental\\graphics                     REGULAR_EXPRESSION "pomdog/experimental/graphics/*")
source_group(experimental\\gui                          REGULAR_EXPRESSION "pomdog/experimental/gui/*")
source_group(experimental\\image_effects                REGULAR_EXPRESSION "pomdog/experimental/image_effects/*")
source_group(experimental\\omage                        REGULAR_EXPRESSION "pomdog/experimental/image/*")
source_group(experimental\\magicavoxel                  REGULAR_EXPRESSION "pomdog/experimental/magicavoxel/*")
source_group(experimental\\particles                    REGULAR_EXPRESSION "pomdog/experimental/particles/*")
source_group(experimental\\particles\\emitter_shapes    REGULAR_EXPRESSION "pomdog/experimental/particles/emitter_shapes/*")
source_group(experimental\\particles\\parameters        REGULAR_EXPRESSION "pomdog/experimental/particles/parameters/*")
source_group(experimental\\random                       REGULAR_EXPRESSION "pomdog/experimental/random/*")
source_group(experimental\\reactive                     REGULAR_EXPRESSION "pomdog/experimental/reactive/*")
source_group(experimental\\reactive\\operators          REGULAR_EXPRESSION "pomdog/experimental/reactive/operators/*")
source_group(experimental\\reactive\\subjects           REGULAR_EXPRESSION "pomdog/experimental/reactive/subjects/*")
source_group(experimental\\skeletal2d                   REGULAR_EXPRESSION "pomdog/experimental/skeletal2d/*")
source_group(experimental\\skeletal2d\\blendtrees       REGULAR_EXPRESSION "pomdog/experimental/skeletal2d/blendtrees/*")
source_group(experimental\\skeletal2d\\tracks           REGULAR_EXPRESSION "pomdog/experimental/skeletal2d/tracks/*")
source_group(experimental\\spine                        REGULAR_EXPRESSION "pomdog/experimental/spine/*")
source_group(experimental\\texture_packer               REGULAR_EXPRESSION "pomdog/experimental/texture_packer/*")
source_group(experimental\\tween                        REGULAR_EXPRESSION "pomdog/experimental/tween/*")

target_sources(pomdog_static PRIVATE
    # NOTE: experimental/async
    ${POMDOG_SRC_DIR}/experimental/async/helpers.hpp
    ${POMDOG_SRC_DIR}/experimental/async/immediate_scheduler.hpp
    ${POMDOG_SRC_DIR}/experimental/async/immediate_scheduler.cpp
    ${POMDOG_SRC_DIR}/experimental/async/queued_scheduler.hpp
    ${POMDOG_SRC_DIR}/experimental/async/queued_scheduler.cpp
    ${POMDOG_SRC_DIR}/experimental/async/scheduler.hpp
    ${POMDOG_SRC_DIR}/experimental/async/scheduler.cpp
    ${POMDOG_SRC_DIR}/experimental/async/task.hpp
    ${POMDOG_SRC_DIR}/experimental/async/task.cpp
)

target_sources(pomdog_static PRIVATE
    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/ecs
        ${POMDOG_SRC_DIR}/experimental/ecs/component_buffer.hpp
        ${POMDOG_SRC_DIR}/experimental/ecs/component_buffer.cpp
        ${POMDOG_SRC_DIR}/experimental/ecs/component_type.hpp
        ${POMDOG_SRC_DIR}/experimental/ecs/component_type_index.hpp
        ${POMDOG_SRC_DIR}/experimental/ecs/component_type_index.cpp
        ${POMDOG_SRC_DIR}/experimental/ecs/entity.hpp
        ${POMDOG_SRC_DIR}/experimental/ecs/entity.cpp
        ${POMDOG_SRC_DIR}/experimental/ecs/entity_archtype.hpp
        ${POMDOG_SRC_DIR}/experimental/ecs/entity_desc.hpp
        ${POMDOG_SRC_DIR}/experimental/ecs/entity_manager.hpp
        ${POMDOG_SRC_DIR}/experimental/ecs/entity_manager.cpp
        ${POMDOG_SRC_DIR}/experimental/ecs/entity_query.hpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/gltf
        ${POMDOG_SRC_DIR}/experimental/gltf/gltf.hpp
        ${POMDOG_SRC_DIR}/experimental/gltf/gltf.cpp
        ${POMDOG_SRC_DIR}/experimental/gltf/gltf_loader.hpp
        ${POMDOG_SRC_DIR}/experimental/gltf/gltf_loader.cpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/graphics
        ${POMDOG_SRC_DIR}/experimental/graphics/basic_effect.hpp
        ${POMDOG_SRC_DIR}/experimental/graphics/basic_effect.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/billboard_effect.hpp
        ${POMDOG_SRC_DIR}/experimental/graphics/billboard_effect.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/font_glyph.hpp
        ${POMDOG_SRC_DIR}/experimental/graphics/line_batch.hpp
        ${POMDOG_SRC_DIR}/experimental/graphics/line_batch.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/polyline_batch.hpp
        ${POMDOG_SRC_DIR}/experimental/graphics/polyline_batch.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/polygon_shape_builder.hpp
        ${POMDOG_SRC_DIR}/experimental/graphics/polygon_shape_builder.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/primitive_batch.hpp
        ${POMDOG_SRC_DIR}/experimental/graphics/primitive_batch.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/sprite_batch.hpp
        ${POMDOG_SRC_DIR}/experimental/graphics/sprite_batch.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/sprite_font.hpp
        ${POMDOG_SRC_DIR}/experimental/graphics/sprite_font.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/sprite_font_loader.hpp
        ${POMDOG_SRC_DIR}/experimental/graphics/sprite_font_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/sprite_line.hpp
        ${POMDOG_SRC_DIR}/experimental/graphics/sprite_line.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/texture2d_view.hpp
        ${POMDOG_SRC_DIR}/experimental/graphics/texture2d_view.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/truetype_font.hpp
        ${POMDOG_SRC_DIR}/experimental/graphics/truetype_font.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/truetype_font_loader.hpp
        ${POMDOG_SRC_DIR}/experimental/graphics/truetype_font_loader.cpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/gui
        ${POMDOG_SRC_DIR}/experimental/gui/check_box.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/check_box.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/context_menu.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/context_menu.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/context_menu_item.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/context_menu_item.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/debug_navigator.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/debug_navigator.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/disclosure_triangle_button.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/disclosure_triangle_button.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/drawing_context.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/drawing_context.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/float_field.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/float_field.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/font_size.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/font_weight.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/hierarchy_sort_order.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/horizontal_alignment.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/horizontal_layout.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/horizontal_layout.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/horizontal_line.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/horizontal_line.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/horizontal_splitter.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/horizontal_splitter.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/int_field.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/int_field.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/list_view.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/list_view.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/list_view_item.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/list_view_item.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/number_field.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/number_field.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/pointer_event_type.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/pointer_point.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/popup_menu.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/popup_menu.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/push_button.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/push_button.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/scene_panel.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/scene_panel.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/scroll_view.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/scroll_view.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/slider.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/slider.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/stack_panel.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/stack_panel.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/text_alignment.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/text_block.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/text_block.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/text_edit.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/text_edit.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/text_field.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/text_field.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/thickness.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/toggle_switch.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/toggle_switch.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/tree_view.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/tree_view.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/ui_event_dispatcher.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/ui_event_dispatcher.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/ui_helper.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/ui_helper.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/vector3_field.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/vector3_field.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/vertical_alignment.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/vertical_layout.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/vertical_layout.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/vertical_scroll_bar.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/vertical_scroll_bar.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/widget.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/widget.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/widget_hierarchy.hpp
        ${POMDOG_SRC_DIR}/experimental/gui/widget_hierarchy.cpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/image
        ${POMDOG_SRC_DIR}/experimental/image/gif_image.hpp
        ${POMDOG_SRC_DIR}/experimental/image/gif_loader.hpp
        ${POMDOG_SRC_DIR}/experimental/image/gif_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/image/image.hpp
        ${POMDOG_SRC_DIR}/experimental/image/image.cpp
        ${POMDOG_SRC_DIR}/experimental/image/svg_loader.hpp
        ${POMDOG_SRC_DIR}/experimental/image/svg_loader.cpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/image_effects
        ${POMDOG_SRC_DIR}/experimental/image_effects/chromatic_aberration.hpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/chromatic_aberration.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/fish_eye_effect.hpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/fish_eye_effect.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/fxaa.hpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/fxaa.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/grayscale_effect.hpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/grayscale_effect.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/image_effect_base.hpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/image_effect_base.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/post_process_compositor.hpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/post_process_compositor.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/retro_crt_effect.hpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/retro_crt_effect.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/screen_quad.hpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/screen_quad.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/sepia_tone_effect.hpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/sepia_tone_effect.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/vignette_effect.hpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/vignette_effect.cpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/magicavoxel
        ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_model.hpp
        ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_model_exporter.hpp
        ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_model_exporter.cpp
        ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_model_loader.hpp
        ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_model_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_chunk_header.hpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/particles
        ${POMDOG_SRC_DIR}/experimental/particles/beam.hpp
        ${POMDOG_SRC_DIR}/experimental/particles/beam_branching.hpp
        ${POMDOG_SRC_DIR}/experimental/particles/beam_emitter.hpp
        ${POMDOG_SRC_DIR}/experimental/particles/beam_system.hpp
        ${POMDOG_SRC_DIR}/experimental/particles/beam_system.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/particle.hpp
        ${POMDOG_SRC_DIR}/experimental/particles/particle_clip.hpp
        ${POMDOG_SRC_DIR}/experimental/particles/particle_clip_loader.hpp
        ${POMDOG_SRC_DIR}/experimental/particles/particle_clip_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/particle_system.hpp
        ${POMDOG_SRC_DIR}/experimental/particles/particle_system.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape.hpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_box.hpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_box.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_cone.hpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_cone.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_hemisphere.hpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_hemisphere.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_sector.hpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_sector.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_sphere.hpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_sphere.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/parameters/particle_curve_key.hpp
        ${POMDOG_SRC_DIR}/experimental/particles/parameters/particle_curve_lerp.hpp
        ${POMDOG_SRC_DIR}/experimental/particles/parameters/particle_parameter.hpp
        ${POMDOG_SRC_DIR}/experimental/particles/parameters/particle_parameter_constant.hpp
        ${POMDOG_SRC_DIR}/experimental/particles/parameters/particle_parameter_curve.hpp
        ${POMDOG_SRC_DIR}/experimental/particles/parameters/particle_parameter_random.hpp
        ${POMDOG_SRC_DIR}/experimental/particles/parameters/particle_parameter_random_curves.hpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/random
        ${POMDOG_SRC_DIR}/experimental/random/xoroshiro128_star_star.hpp
        ${POMDOG_SRC_DIR}/experimental/random/xoroshiro128_star_star.cpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/reactive
        ${POMDOG_SRC_DIR}/experimental/reactive/observable.hpp
        ${POMDOG_SRC_DIR}/experimental/reactive/observable_base.hpp
        ${POMDOG_SRC_DIR}/experimental/reactive/observable_builder.hpp
        ${POMDOG_SRC_DIR}/experimental/reactive/observer.hpp
        ${POMDOG_SRC_DIR}/experimental/reactive/subscriber.hpp
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/buffer_operator.hpp
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/debounce_operator.hpp
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/delay_operator.hpp
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/distinct_operator.hpp
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/do_operator.hpp
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/filter_operator.hpp
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/first_operator.hpp
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/last_operator.hpp
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/map_operator.hpp
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/merge_operator.hpp
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/scan_operator.hpp
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/skip_last_operator.hpp
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/skip_operator.hpp
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/take_operator.hpp
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/timeout_operator.hpp
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/zip_operator.hpp
        ${POMDOG_SRC_DIR}/experimental/reactive/subjects/key_down_subject.hpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/skeletal2d
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animation_clip.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animation_clip.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animation_state.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animation_state.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animation_system.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animation_system.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animation_time_interval.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animator.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animator.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/compressed_float.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/joint_index.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/joint_pose.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/joint.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/rigid_slot.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skeleton_helper.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skeleton_helper.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skeleton_pose.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skeleton_pose.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skeleton.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skeleton.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skin.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skin.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skinned_mesh_part.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skinned_mesh.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skinned_vertex.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_additive_node.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_additive_node.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_blend_input_type.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_blend_input.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_clip_node.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_clip_node.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_cross_fade_node.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_cross_fade_node.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_graph_weight_collection.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_graph_weight_collection.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_graph.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_lerp_node.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_lerp_node.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_node.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_node.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/weight_blending_helper.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/weight_blending_helper.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/animation_key_helper.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/animation_track.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/animation_track.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/rotation_track.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/scale_track.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/sprite_animation_track.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/sprite_animation_track.hpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/translation_track.hpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/spine
        ${POMDOG_SRC_DIR}/experimental/spine/animation_graph_builder.cpp
        ${POMDOG_SRC_DIR}/experimental/spine/animation_graph_builder.hpp
        ${POMDOG_SRC_DIR}/experimental/spine/animation_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/spine/animation_loader.hpp
        ${POMDOG_SRC_DIR}/experimental/spine/skeleton_desc_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/spine/skeleton_desc_loader.hpp
        ${POMDOG_SRC_DIR}/experimental/spine/skeleton_desc.hpp
        ${POMDOG_SRC_DIR}/experimental/spine/skeleton_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/spine/skeleton_loader.hpp
        ${POMDOG_SRC_DIR}/experimental/spine/skin_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/spine/skin_loader.hpp
        ${POMDOG_SRC_DIR}/experimental/spine/skinned_mesh_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/spine/skinned_mesh_loader.hpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/texture_packer
        ${POMDOG_SRC_DIR}/experimental/texture_packer/texture_atlas_generator.cpp
        ${POMDOG_SRC_DIR}/experimental/texture_packer/texture_atlas_generator.hpp
        ${POMDOG_SRC_DIR}/experimental/texture_packer/texture_atlas_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/texture_packer/texture_atlas_loader.hpp
        ${POMDOG_SRC_DIR}/experimental/texture_packer/texture_atlas.hpp
        ${POMDOG_SRC_DIR}/experimental/texture_packer/texture_region.hpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/tween
        ${POMDOG_SRC_DIR}/experimental/tween/easing_helper.hpp
        ${POMDOG_SRC_DIR}/experimental/tween/easing_helper.cpp
    >
)
