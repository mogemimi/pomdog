// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Spine/SkeletonDesc.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <memory>
#include <string>
#include <tuple>

namespace Pomdog::Spine {

class SkeletonDescLoader final {
public:
    [[nodiscard]] static std::tuple<SkeletonDesc, std::shared_ptr<Error>>
    Load(const std::string& filePath);
};

} // namespace Pomdog::Spine
