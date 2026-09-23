add_library(pomdog_experimental_skeletal2d STATIC)
add_library(pomdog::experimental_skeletal2d ALIAS pomdog_experimental_skeletal2d)

target_link_libraries(pomdog_experimental_skeletal2d
    PUBLIC
        pomdog_math
    PRIVATE
        pomdog_build_settings
)

source_group(experimental/skeletal2d REGULAR_EXPRESSION "pomdog/experimental/skeletal2d/*")
source_group(experimental/skeletal2d/blendtrees REGULAR_EXPRESSION "pomdog/experimental/skeletal2d/blendtrees/*")
source_group(experimental/skeletal2d/tracks REGULAR_EXPRESSION "pomdog/experimental/skeletal2d/tracks/*")

target_sources(pomdog_experimental_skeletal2d PRIVATE
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
)
