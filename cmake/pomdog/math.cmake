source_group(math REGULAR_EXPRESSION "pomdog/math/*")

target_sources(pomdog_static PRIVATE
    # NOTE: math
    ${POMDOG_SRC_DIR}/math/bounding_box.cpp
    ${POMDOG_SRC_DIR}/math/bounding_box.h
    ${POMDOG_SRC_DIR}/math/bounding_box2d.cpp
    ${POMDOG_SRC_DIR}/math/bounding_box2d.h
    ${POMDOG_SRC_DIR}/math/bounding_circle.cpp
    ${POMDOG_SRC_DIR}/math/bounding_circle.h
    ${POMDOG_SRC_DIR}/math/bounding_frustum.cpp
    ${POMDOG_SRC_DIR}/math/bounding_frustum.h
    ${POMDOG_SRC_DIR}/math/bounding_sphere.cpp
    ${POMDOG_SRC_DIR}/math/bounding_sphere.h
    ${POMDOG_SRC_DIR}/math/color.cpp
    ${POMDOG_SRC_DIR}/math/color.h
    ${POMDOG_SRC_DIR}/math/containment_type.h
    ${POMDOG_SRC_DIR}/math/degree.cpp
    ${POMDOG_SRC_DIR}/math/degree.h
    ${POMDOG_SRC_DIR}/math/forward_declarations.h
    ${POMDOG_SRC_DIR}/math/math.cpp
    ${POMDOG_SRC_DIR}/math/math.h
    ${POMDOG_SRC_DIR}/math/matrix2x2.cpp
    ${POMDOG_SRC_DIR}/math/matrix2x2.h
    ${POMDOG_SRC_DIR}/math/matrix3x2.cpp
    ${POMDOG_SRC_DIR}/math/matrix3x2.h
    ${POMDOG_SRC_DIR}/math/matrix3x3.cpp
    ${POMDOG_SRC_DIR}/math/matrix3x3.h
    ${POMDOG_SRC_DIR}/math/matrix4x4.cpp
    ${POMDOG_SRC_DIR}/math/matrix4x4.h
    ${POMDOG_SRC_DIR}/math/plane_intersection_type.h
    ${POMDOG_SRC_DIR}/math/plane.cpp
    ${POMDOG_SRC_DIR}/math/plane.h
    ${POMDOG_SRC_DIR}/math/point2d.cpp
    ${POMDOG_SRC_DIR}/math/point2d.h
    ${POMDOG_SRC_DIR}/math/point3d.cpp
    ${POMDOG_SRC_DIR}/math/point3d.h
    ${POMDOG_SRC_DIR}/math/quaternion.cpp
    ${POMDOG_SRC_DIR}/math/quaternion.h
    ${POMDOG_SRC_DIR}/math/radian.cpp
    ${POMDOG_SRC_DIR}/math/radian.h
    ${POMDOG_SRC_DIR}/math/ray.cpp
    ${POMDOG_SRC_DIR}/math/ray.h
    ${POMDOG_SRC_DIR}/math/rectangle.cpp
    ${POMDOG_SRC_DIR}/math/rectangle.h
    ${POMDOG_SRC_DIR}/math/tagged_arithmetic.h
    ${POMDOG_SRC_DIR}/math/vector2.cpp
    ${POMDOG_SRC_DIR}/math/vector2.h
    ${POMDOG_SRC_DIR}/math/vector3.cpp
    ${POMDOG_SRC_DIR}/math/vector3.h
    ${POMDOG_SRC_DIR}/math/vector4.cpp
    ${POMDOG_SRC_DIR}/math/vector4.h
)
