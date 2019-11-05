// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Skeletal2D/SkinnedMeshPart.hpp"
#include "Pomdog/Experimental/Skeletal2D/SkinnedVertex.hpp"
#include <cstdint>
#include <memory>
#include <vector>

namespace Pomdog::Skeletal2D {

class SkinnedMesh final {
public:
    std::vector<Skeletal2D::SkinnedVertex> Vertices;
    std::vector<std::uint16_t> Indices;
    std::vector<Skeletal2D::SkinnedMeshPart> MeshParts;
};

} // namespace Pomdog::Skeletal2D
