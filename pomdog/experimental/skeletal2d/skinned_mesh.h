// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/skeletal2d/skinned_mesh_part.h"
#include "pomdog/experimental/skeletal2d/skinned_vertex.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {

class SkinnedMesh final {
public:
    std::vector<skeletal2d::SkinnedVertex> Vertices;
    std::vector<std::uint16_t> Indices;
    std::vector<skeletal2d::SkinnedMeshPart> MeshParts;
};

} // namespace pomdog::skeletal2d
