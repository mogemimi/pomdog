// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/EffectVariable.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace Pomdog {

struct EffectConstantDescription final {
    std::vector<EffectVariable> Variables;
    std::string Name;
    std::uint32_t ByteSize;
};

} // namespace Pomdog
