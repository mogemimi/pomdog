source_group(experimental/async                     REGULAR_EXPRESSION "pomdog/experimental/async/*")
source_group(experimental/ecs                       REGULAR_EXPRESSION "pomdog/experimental/ecs/*")
source_group(experimental/gltf                      REGULAR_EXPRESSION "pomdog/experimental/gltf/*")
source_group(experimental/graphics                  REGULAR_EXPRESSION "pomdog/experimental/graphics/*")
source_group(experimental/gui                       REGULAR_EXPRESSION "pomdog/experimental/gui/*")
source_group(experimental/image_effects             REGULAR_EXPRESSION "pomdog/experimental/image_effects/*")
source_group(experimental/image                     REGULAR_EXPRESSION "pomdog/experimental/image/*")
source_group(experimental/magicavoxel               REGULAR_EXPRESSION "pomdog/experimental/magicavoxel/*")
source_group(experimental/particles                 REGULAR_EXPRESSION "pomdog/experimental/particles/*")
source_group(experimental/particles/emitter_shapes  REGULAR_EXPRESSION "pomdog/experimental/particles/emitter_shapes/*")
source_group(experimental/particles/parameters      REGULAR_EXPRESSION "pomdog/experimental/particles/parameters/*")
source_group(experimental/random                    REGULAR_EXPRESSION "pomdog/experimental/random/*")
source_group(experimental/reactive                  REGULAR_EXPRESSION "pomdog/experimental/reactive/*")
source_group(experimental/reactive/operators        REGULAR_EXPRESSION "pomdog/experimental/reactive/operators/*")
source_group(experimental/reactive/subjects         REGULAR_EXPRESSION "pomdog/experimental/reactive/subjects/*")
source_group(experimental/skeletal2d                REGULAR_EXPRESSION "pomdog/experimental/skeletal2d/*")
source_group(experimental/skeletal2d/blendtrees     REGULAR_EXPRESSION "pomdog/experimental/skeletal2d/blendtrees/*")
source_group(experimental/skeletal2d/tracks         REGULAR_EXPRESSION "pomdog/experimental/skeletal2d/tracks/*")
source_group(experimental/spine                     REGULAR_EXPRESSION "pomdog/experimental/spine/*")
source_group(experimental/texture_packer            REGULAR_EXPRESSION "pomdog/experimental/texture_packer/*")
source_group(experimental/tween                     REGULAR_EXPRESSION "pomdog/experimental/tween/*")

target_sources(pomdog_static PRIVATE
    # NOTE: experimental/async
    ${POMDOG_SRC_DIR}/experimental/async/helpers.h
    ${POMDOG_SRC_DIR}/experimental/async/immediate_scheduler.h
    ${POMDOG_SRC_DIR}/experimental/async/immediate_scheduler.cpp
    ${POMDOG_SRC_DIR}/experimental/async/queued_scheduler.h
    ${POMDOG_SRC_DIR}/experimental/async/queued_scheduler.cpp
    ${POMDOG_SRC_DIR}/experimental/async/scheduler.h
    ${POMDOG_SRC_DIR}/experimental/async/scheduler.cpp
    ${POMDOG_SRC_DIR}/experimental/async/task.h
    ${POMDOG_SRC_DIR}/experimental/async/task.cpp
)

target_sources(pomdog_static PRIVATE
    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/ecs
        ${POMDOG_SRC_DIR}/experimental/ecs/component_buffer.h
        ${POMDOG_SRC_DIR}/experimental/ecs/component_buffer.cpp
        ${POMDOG_SRC_DIR}/experimental/ecs/component_type.h
        ${POMDOG_SRC_DIR}/experimental/ecs/component_type_index.h
        ${POMDOG_SRC_DIR}/experimental/ecs/component_type_index.cpp
        ${POMDOG_SRC_DIR}/experimental/ecs/entity.h
        ${POMDOG_SRC_DIR}/experimental/ecs/entity.cpp
        ${POMDOG_SRC_DIR}/experimental/ecs/entity_archtype.h
        ${POMDOG_SRC_DIR}/experimental/ecs/entity_desc.h
        ${POMDOG_SRC_DIR}/experimental/ecs/entity_manager.h
        ${POMDOG_SRC_DIR}/experimental/ecs/entity_manager.cpp
        ${POMDOG_SRC_DIR}/experimental/ecs/entity_query.h
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/gltf
        ${POMDOG_SRC_DIR}/experimental/gltf/gltf.h
        ${POMDOG_SRC_DIR}/experimental/gltf/gltf.cpp
        ${POMDOG_SRC_DIR}/experimental/gltf/gltf_loader.h
        ${POMDOG_SRC_DIR}/experimental/gltf/gltf_loader.cpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/graphics
        ${POMDOG_SRC_DIR}/experimental/graphics/basic_effect.h
        ${POMDOG_SRC_DIR}/experimental/graphics/basic_effect.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/billboard_effect.h
        ${POMDOG_SRC_DIR}/experimental/graphics/billboard_effect.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/font_glyph.h
        ${POMDOG_SRC_DIR}/experimental/graphics/line_batch.h
        ${POMDOG_SRC_DIR}/experimental/graphics/line_batch.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/polyline_batch.h
        ${POMDOG_SRC_DIR}/experimental/graphics/polyline_batch.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/polygon_shape_builder.h
        ${POMDOG_SRC_DIR}/experimental/graphics/polygon_shape_builder.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/primitive_batch.h
        ${POMDOG_SRC_DIR}/experimental/graphics/primitive_batch.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/sprite_batch.h
        ${POMDOG_SRC_DIR}/experimental/graphics/sprite_batch.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/sprite_font.h
        ${POMDOG_SRC_DIR}/experimental/graphics/sprite_font.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/sprite_font_loader.h
        ${POMDOG_SRC_DIR}/experimental/graphics/sprite_font_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/sprite_line.h
        ${POMDOG_SRC_DIR}/experimental/graphics/sprite_line.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/texture2d_view.h
        ${POMDOG_SRC_DIR}/experimental/graphics/texture2d_view.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/truetype_font.h
        ${POMDOG_SRC_DIR}/experimental/graphics/truetype_font.cpp
        ${POMDOG_SRC_DIR}/experimental/graphics/truetype_font_loader.h
        ${POMDOG_SRC_DIR}/experimental/graphics/truetype_font_loader.cpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/gui
        ${POMDOG_SRC_DIR}/experimental/gui/check_box.h
        ${POMDOG_SRC_DIR}/experimental/gui/check_box.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/context_menu.h
        ${POMDOG_SRC_DIR}/experimental/gui/context_menu.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/context_menu_item.h
        ${POMDOG_SRC_DIR}/experimental/gui/context_menu_item.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/debug_navigator.h
        ${POMDOG_SRC_DIR}/experimental/gui/debug_navigator.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/disclosure_triangle_button.h
        ${POMDOG_SRC_DIR}/experimental/gui/disclosure_triangle_button.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/drawing_context.h
        ${POMDOG_SRC_DIR}/experimental/gui/drawing_context.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/float_field.h
        ${POMDOG_SRC_DIR}/experimental/gui/float_field.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/font_size.h
        ${POMDOG_SRC_DIR}/experimental/gui/font_weight.h
        ${POMDOG_SRC_DIR}/experimental/gui/hierarchy_sort_order.h
        ${POMDOG_SRC_DIR}/experimental/gui/horizontal_alignment.h
        ${POMDOG_SRC_DIR}/experimental/gui/horizontal_layout.h
        ${POMDOG_SRC_DIR}/experimental/gui/horizontal_layout.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/horizontal_line.h
        ${POMDOG_SRC_DIR}/experimental/gui/horizontal_line.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/horizontal_splitter.h
        ${POMDOG_SRC_DIR}/experimental/gui/horizontal_splitter.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/int_field.h
        ${POMDOG_SRC_DIR}/experimental/gui/int_field.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/list_view.h
        ${POMDOG_SRC_DIR}/experimental/gui/list_view.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/list_view_item.h
        ${POMDOG_SRC_DIR}/experimental/gui/list_view_item.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/number_field.h
        ${POMDOG_SRC_DIR}/experimental/gui/number_field.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/pointer_event_type.h
        ${POMDOG_SRC_DIR}/experimental/gui/pointer_point.h
        ${POMDOG_SRC_DIR}/experimental/gui/popup_menu.h
        ${POMDOG_SRC_DIR}/experimental/gui/popup_menu.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/push_button.h
        ${POMDOG_SRC_DIR}/experimental/gui/push_button.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/scene_panel.h
        ${POMDOG_SRC_DIR}/experimental/gui/scene_panel.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/scroll_view.h
        ${POMDOG_SRC_DIR}/experimental/gui/scroll_view.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/slider.h
        ${POMDOG_SRC_DIR}/experimental/gui/slider.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/stack_panel.h
        ${POMDOG_SRC_DIR}/experimental/gui/stack_panel.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/text_alignment.h
        ${POMDOG_SRC_DIR}/experimental/gui/text_block.h
        ${POMDOG_SRC_DIR}/experimental/gui/text_block.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/text_edit.h
        ${POMDOG_SRC_DIR}/experimental/gui/text_edit.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/text_field.h
        ${POMDOG_SRC_DIR}/experimental/gui/text_field.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/thickness.h
        ${POMDOG_SRC_DIR}/experimental/gui/toggle_switch.h
        ${POMDOG_SRC_DIR}/experimental/gui/toggle_switch.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/tree_view.h
        ${POMDOG_SRC_DIR}/experimental/gui/tree_view.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/ui_event_dispatcher.h
        ${POMDOG_SRC_DIR}/experimental/gui/ui_event_dispatcher.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/ui_helper.h
        ${POMDOG_SRC_DIR}/experimental/gui/ui_helper.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/vector3_field.h
        ${POMDOG_SRC_DIR}/experimental/gui/vector3_field.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/vertical_alignment.h
        ${POMDOG_SRC_DIR}/experimental/gui/vertical_layout.h
        ${POMDOG_SRC_DIR}/experimental/gui/vertical_layout.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/vertical_scroll_bar.h
        ${POMDOG_SRC_DIR}/experimental/gui/vertical_scroll_bar.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/widget.h
        ${POMDOG_SRC_DIR}/experimental/gui/widget.cpp
        ${POMDOG_SRC_DIR}/experimental/gui/widget_hierarchy.h
        ${POMDOG_SRC_DIR}/experimental/gui/widget_hierarchy.cpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/image
        ${POMDOG_SRC_DIR}/experimental/image/image.h
        ${POMDOG_SRC_DIR}/experimental/image/image.cpp
        ${POMDOG_SRC_DIR}/experimental/image/svg_loader.h
        ${POMDOG_SRC_DIR}/experimental/image/svg_loader.cpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/image_effects
        ${POMDOG_SRC_DIR}/experimental/image_effects/chromatic_aberration.h
        ${POMDOG_SRC_DIR}/experimental/image_effects/chromatic_aberration.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/fish_eye_effect.h
        ${POMDOG_SRC_DIR}/experimental/image_effects/fish_eye_effect.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/fxaa.h
        ${POMDOG_SRC_DIR}/experimental/image_effects/fxaa.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/grayscale_effect.h
        ${POMDOG_SRC_DIR}/experimental/image_effects/grayscale_effect.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/image_effect_base.h
        ${POMDOG_SRC_DIR}/experimental/image_effects/image_effect_base.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/post_process_compositor.h
        ${POMDOG_SRC_DIR}/experimental/image_effects/post_process_compositor.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/retro_crt_effect.h
        ${POMDOG_SRC_DIR}/experimental/image_effects/retro_crt_effect.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/screen_quad.h
        ${POMDOG_SRC_DIR}/experimental/image_effects/screen_quad.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/sepia_tone_effect.h
        ${POMDOG_SRC_DIR}/experimental/image_effects/sepia_tone_effect.cpp
        ${POMDOG_SRC_DIR}/experimental/image_effects/vignette_effect.h
        ${POMDOG_SRC_DIR}/experimental/image_effects/vignette_effect.cpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/magicavoxel
        ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_model.h
        ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_model_exporter.h
        ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_model_exporter.cpp
        ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_model_loader.h
        ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_model_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/magicavoxel/vox_chunk_header.h
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/particles
        ${POMDOG_SRC_DIR}/experimental/particles/beam.h
        ${POMDOG_SRC_DIR}/experimental/particles/beam_branching.h
        ${POMDOG_SRC_DIR}/experimental/particles/beam_emitter.h
        ${POMDOG_SRC_DIR}/experimental/particles/beam_system.h
        ${POMDOG_SRC_DIR}/experimental/particles/beam_system.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/particle.h
        ${POMDOG_SRC_DIR}/experimental/particles/particle_clip.h
        ${POMDOG_SRC_DIR}/experimental/particles/particle_clip_loader.h
        ${POMDOG_SRC_DIR}/experimental/particles/particle_clip_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/particle_system.h
        ${POMDOG_SRC_DIR}/experimental/particles/particle_system.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape.h
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_box.h
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_box.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_cone.h
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_cone.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_hemisphere.h
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_hemisphere.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_sector.h
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_sector.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_sphere.h
        ${POMDOG_SRC_DIR}/experimental/particles/emitter_shapes/particle_emitter_shape_sphere.cpp
        ${POMDOG_SRC_DIR}/experimental/particles/parameters/particle_curve_key.h
        ${POMDOG_SRC_DIR}/experimental/particles/parameters/particle_curve_lerp.h
        ${POMDOG_SRC_DIR}/experimental/particles/parameters/particle_parameter.h
        ${POMDOG_SRC_DIR}/experimental/particles/parameters/particle_parameter_constant.h
        ${POMDOG_SRC_DIR}/experimental/particles/parameters/particle_parameter_curve.h
        ${POMDOG_SRC_DIR}/experimental/particles/parameters/particle_parameter_random.h
        ${POMDOG_SRC_DIR}/experimental/particles/parameters/particle_parameter_random_curves.h
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/random
        ${POMDOG_SRC_DIR}/experimental/random/xoroshiro128_star_star.h
        ${POMDOG_SRC_DIR}/experimental/random/xoroshiro128_star_star.cpp
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/reactive
        ${POMDOG_SRC_DIR}/experimental/reactive/observable.h
        ${POMDOG_SRC_DIR}/experimental/reactive/observable_base.h
        ${POMDOG_SRC_DIR}/experimental/reactive/observable_builder.h
        ${POMDOG_SRC_DIR}/experimental/reactive/observer.h
        ${POMDOG_SRC_DIR}/experimental/reactive/subscriber.h
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/buffer_operator.h
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/debounce_operator.h
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/delay_operator.h
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/distinct_operator.h
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/do_operator.h
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/filter_operator.h
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/first_operator.h
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/last_operator.h
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/map_operator.h
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/merge_operator.h
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/scan_operator.h
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/skip_last_operator.h
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/skip_operator.h
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/take_operator.h
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/timeout_operator.h
        ${POMDOG_SRC_DIR}/experimental/reactive/operators/zip_operator.h
        ${POMDOG_SRC_DIR}/experimental/reactive/subjects/key_down_subject.h
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/skeletal2d
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animation_clip.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animation_clip.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animation_state.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animation_state.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animation_system.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animation_system.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animation_time_interval.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animator.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/animator.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/compressed_float.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/joint_index.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/joint_pose.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/joint.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/rigid_slot.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skeleton_helper.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skeleton_helper.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skeleton_pose.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skeleton_pose.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skeleton.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skeleton.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skin.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skin.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skinned_mesh_part.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skinned_mesh.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/skinned_vertex.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_additive_node.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_additive_node.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_blend_input_type.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_blend_input.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_clip_node.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_clip_node.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_cross_fade_node.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_cross_fade_node.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_graph_weight_collection.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_graph_weight_collection.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_graph.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_lerp_node.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_lerp_node.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_node.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/animation_node.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/weight_blending_helper.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/blendtrees/weight_blending_helper.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/animation_key_helper.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/animation_track.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/animation_track.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/rotation_track.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/scale_track.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/sprite_animation_track.cpp
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/sprite_animation_track.h
        ${POMDOG_SRC_DIR}/experimental/skeletal2d/tracks/translation_track.h
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/spine
        ${POMDOG_SRC_DIR}/experimental/spine/animation_graph_builder.cpp
        ${POMDOG_SRC_DIR}/experimental/spine/animation_graph_builder.h
        ${POMDOG_SRC_DIR}/experimental/spine/animation_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/spine/animation_loader.h
        ${POMDOG_SRC_DIR}/experimental/spine/skeleton_desc_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/spine/skeleton_desc_loader.h
        ${POMDOG_SRC_DIR}/experimental/spine/skeleton_desc.h
        ${POMDOG_SRC_DIR}/experimental/spine/skeleton_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/spine/skeleton_loader.h
        ${POMDOG_SRC_DIR}/experimental/spine/skin_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/spine/skin_loader.h
        ${POMDOG_SRC_DIR}/experimental/spine/skinned_mesh_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/spine/skinned_mesh_loader.h
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/texture_packer
        ${POMDOG_SRC_DIR}/experimental/texture_packer/texture_atlas_generator.cpp
        ${POMDOG_SRC_DIR}/experimental/texture_packer/texture_atlas_generator.h
        ${POMDOG_SRC_DIR}/experimental/texture_packer/texture_atlas_loader.cpp
        ${POMDOG_SRC_DIR}/experimental/texture_packer/texture_atlas_loader.h
        ${POMDOG_SRC_DIR}/experimental/texture_packer/texture_atlas.h
        ${POMDOG_SRC_DIR}/experimental/texture_packer/texture_region.h
    >

    $<$<BOOL:${POMDOG_USE_EXPERIMENTAL}>:
        # NOTE: experimental/tween
        ${POMDOG_SRC_DIR}/experimental/tween/easing_helper.h
        ${POMDOG_SRC_DIR}/experimental/tween/easing_helper.cpp
    >
)
