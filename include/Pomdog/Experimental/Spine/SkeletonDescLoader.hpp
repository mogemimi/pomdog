// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/Spine/SkeletonDesc.hpp"
#include "Pomdog/Utility/Errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Spine {

class SkeletonDescLoader final {
public:
    [[nodiscard]] static std::tuple<SkeletonDesc, std::unique_ptr<Error>>
    Load(const std::string& filePath);
};

} // namespace Pomdog::Spine
