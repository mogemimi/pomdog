// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/TexturePacker/TextureAtlas.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Utility/Errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Skeletal2D {
class SkeletonPose;
class SkinnedMesh;
} // namespace Pomdog::Skeletal2D

namespace Pomdog::Spine {

class SkeletonDesc;

[[nodiscard]] std::tuple<Skeletal2D::SkinnedMesh, std::unique_ptr<Error>>
CreateSkinnedMesh(
    const std::vector<Matrix3x2>& bindPosesInGlobal,
    const SkeletonDesc& skeletonDesc,
    const TexturePacker::TextureAtlas& textureAtlas,
    const Vector2& textureSize,
    const std::string& skinName);

} // namespace Pomdog::Spine
