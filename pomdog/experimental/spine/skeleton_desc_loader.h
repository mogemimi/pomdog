// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/spine/skeleton_desc.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::spine {

class SkeletonDescLoader final {
public:
    [[nodiscard]] static std::tuple<SkeletonDesc, std::unique_ptr<Error>>
    Load(const std::string& filePath);
};

} // namespace pomdog::spine
