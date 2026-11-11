// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/skinned_mesh_part.h"
#include "pomdog/experimental/skeletal2d/skinned_vertex.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {

class SkinnedMesh final {
public:
    std::vector<skeletal2d::SkinnedVertex> vertices;
    std::vector<u16> indices;
    std::vector<skeletal2d::SkinnedMeshPart> meshParts;
};

} // namespace pomdog::skeletal2d
