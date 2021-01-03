// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/Skeletal2D/SkinnedMeshPart.hpp"
#include "Pomdog/Experimental/Skeletal2D/SkinnedVertex.hpp"

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
