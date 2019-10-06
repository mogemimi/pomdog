// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "SkeletonDesc.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <memory>
#include <tuple>

namespace Pomdog::Spine {

class SkeletonDescLoader final {
public:
    [[nodiscard]] static std::tuple<SkeletonDesc, std::shared_ptr<Error>>
    Load(const std::string& filePath);
};

} // namespace Pomdog::Spine
