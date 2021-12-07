// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/skeletal2d/skinned_mesh_part.hpp"
#include "pomdog/experimental/skeletal2d/skinned_vertex.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Skeletal2D {

class SkinnedMesh final {
public:
    std::vector<Skeletal2D::SkinnedVertex> Vertices;
    std::vector<std::uint16_t> Indices;
    std::vector<Skeletal2D::SkinnedMeshPart> MeshParts;
};

} // namespace Pomdog::Skeletal2D
