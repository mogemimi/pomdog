add_library(pomdog_experimental_particles STATIC)
add_library(pomdog::experimental_particles ALIAS pomdog_experimental_particles)

target_link_libraries(pomdog_experimental_particles
    PUBLIC
        pomdog_math
        pomdog_random
    PRIVATE
        pomdog_build_settings
        pomdog_content_utility
        pomdog_vfs
)

source_group(experimental/particles REGULAR_EXPRESSION "pomdog/experimental/particles/*")
source_group(experimental/particles/emitter_shapes REGULAR_EXPRESSION "pomdog/experimental/particles/emitter_shapes/*")
source_group(experimental/particles/parameters REGULAR_EXPRESSION "pomdog/experimental/particles/parameters/*")

target_sources(pomdog_experimental_particles PRIVATE
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
)

target_include_directories(pomdog_experimental_particles PRIVATE
    ${POMDOG_DIR}/thirdparty/rapidjson/include
)
