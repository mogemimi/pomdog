// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {
class Skeleton;
} // namespace pomdog::skeletal2d

namespace pomdog::spine {

class BoneDesc;

[[nodiscard]] skeletal2d::Skeleton
CreateSkeleton(const std::vector<BoneDesc>& bones);

} // namespace pomdog::spine
